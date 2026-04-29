# IME Keys for US v0.1.0 Task

このファイルは、初期実装時の要求をいつでも確認できるようにするための作業指示です。

関連する詳細記録:

- `docs/original-request.md`: 初回要求を仕様として整理した記録。
- `docs/implementation-log.md`: v0.1.0 初期実装で実際に行った手順の記録。
- `docs/issues.md`: 後で解決する懸案の記録。

## 目的

`IME Keys for US` は、日本語ユーザーが US キーボード上で明示的な IME OFF / IME ON キーを使えるようにする、狭いスコープのネイティブ Windows アプリケーションです。

左 Alt 単独タップを IME OFF、右 Alt 単独タップを IME ON として扱います。IME トグルは主要動作として実装しません。

## v0.1.0 の必須動作

| Input | Behavior |
|---|---|
| Left Alt single tap | IME OFF after double-tap timeout |
| Right Alt single tap | IME ON after double-tap timeout |
| Left Alt tap, then Left Alt down within timeout | Cancel IME OFF and enter normal Left Alt mode |
| Right Alt tap, then Right Alt down within timeout | Cancel IME ON and enter normal Right Alt mode |
| Left Alt + another key | Normal Left Alt shortcut |
| Right Alt + another key | Normal Right Alt shortcut |
| Right Alt held, then Left Alt | Standalone Left Alt |
| Left Alt held, then Right Alt | Standalone Right Alt |

## 実装制約

- C++17 以降、Win32 API、CMake を使う。
- AutoHotkey、.NET、Electron、WebView、ネットワーク、テレメトリ、アップデータは使わない。
- `WH_KEYBOARD_LL` を使い、フック callback は軽量に保つ。
- `SendInput` は抑止した Alt 入力の再送にだけ使う。
- IME 制御は `src/ime_controller.*` に隔離する。
- TSF は v0.1.0 では実装しない。
- standalone Alt fallback は double-tap と cross Alt で提供する。
- 署名素材、証明書、秘密鍵、PFX、パスワードは commit しない。

## リリース作業

- 初期バージョンは `0.1.0`。
- 開発ブランチは `devel`、PR 先は `main`。
- PR タイトルは `Build initial IME Keys for US v0.1.0`。
- GitHub Release はレビュー後に `v0.1.0` tag から作成する。
- winget manifest は GitHub Release URL と SHA256 が確定してから完成させる。
