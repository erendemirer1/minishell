# minishell

Bu proje, POSIX davranışını temel alan küçük bir kabuk (shell) implementasyonudur.  
Amaç sadece komut çalıştırmak değil; lexer/parsing, süreç yönetimi, redirection, heredoc ve environment mutasyonlarını tek bir akış içinde doğru şekilde yönetmektir.

## Kapsam

Bu minishell şu başlıkları hedefler:

- İnteraktif komut satırı (`readline` + history)
- Token üretimi ve komutların bağlı liste (`t_cmd`) üzerinde temsil edilmesi
- Pipe zincirleri (`|`) ile çoklu süreç yürütme
- Redirection operatörleri:
  - `<`
  - `>`
  - `>>`
  - `<<` (heredoc)
- Ortam değişkeni genişletme (`$VAR`, `$?`)
- Builtin komutlar:
  - `cd`
  - `pwd`
  - `echo`
  - `env`
  - `export`
  - `unset`
  - `exit`
- `PATH` çözümleme ve `execve` ile external command çalıştırma
- `SIGINT` / `SIGQUIT` yönetimi

## Derleme ve Çalıştırma

```bash
make
./minishell
```

Kısa yollar:

```bash
make run    # re + çalıştır + fclean
make v      # valgrind ile çalıştır
make n      # norminette kontrolü
```

> Not: Derleme `readline` kütüphanesine bağlıdır.

## Mimari Özeti

### 1) Giriş ve Prompt

`minishell.c` döngüsü her iterasyonda:

1. Güncel çalışma dizinini ve HOME bilgisini toplar
2. Prompt string’ini üretir
3. `readline` ile kullanıcı girdisini alır
4. Boş olmayan girdileri history’ye ekler

### 2) Parse Katmanı

Komut satırı `create_cmd` akışıyla `t_cmd` bağlı listesine dönüştürülür:

- Normal kelimeler `token = NONE`
- Operatörler ayrı node olarak tutulur (`PIPE`, `INPUT`, `HEREDOC`, `WRITE`, `REWRITE`)
- Quote ve `$` genişletmesi parsing aşamasında işlenir

Bu tasarım, redirection ve execution aşamasında “token tüketme” işini basitleştirir.

### 3) Sözdizimi Kontrolü + Heredoc Hazırlığı

Execution öncesi iki kritik adım var:

- Geçersiz token dizilimlerini yakalama (ör. ardışık `|` veya eksik operand)
- Heredoc bloklarını önceden okuyup pipe fd’lerine bağlama

Böylece gerçek execute aşamasına girildiğinde giriş kaynakları hazır olur.

### 4) Redirection Uygulaması

`redirection.c` içinde komut listesi üzerinde ilerlenir:

- İlgili dosya/fd açılır
- Gerekli `dup2` bağlamaları yapılır
- Kullanılmış redirection token node’ları listeden temizlenir

Bu sayede geriye sadece yürütülecek komut argümanları kalır.

### 5) Yürütme Modeli

`exec.c` tarafında:

- Pipe varsa süreçler `fork + pipe + dup2` ile zincirlenir
- Builtin ve external command ayrımı yapılır
- External command için `PATH` çözülüp `execve` çağrılır
- Parent süreç `waitpid` ile exit status toplar

`$?` değeri bu status üzerinden güncellenir.

## Veri Yapıları

### `t_cmd`
Tokenize edilmiş komut akışı için tek yönlü bağlı liste.

### `t_env`
Environment değişkenleri için key/value listesi; `export`, `unset`, `cd` gibi built-in’lerde aktif olarak mutasyona uğrar.

### `t_ms`
Uygulamanın çalışma anındaki tüm state’ini taşır:

- aktif komut listesi
- env listesi
- heredoc fd’leri
- son status kodu
- geçici parse string’leri

## Davranış Notları

- `cd` çalıştığında `PWD`/`OLDPWD` güncellenir
- `echo`, birden fazla `-n` varyasyonunu destekler (`-n`, `-nnn`, ...)
- `export` argümansız çağrıldığında ortamı sıralı biçimde basar
- PATH içinde bulunamayan komutlarda uygun hata kodu döner

## Teknik Odak

Bu repo “girdi al, komut çalıştır” seviyesinden çok daha fazlasını hedefler:  
asıl odak, shell davranışını küçük bir çekirdekte deterministik ve yönetilebilir bir state makinesi olarak modelleyebilmektir.
