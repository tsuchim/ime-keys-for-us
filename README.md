# IME Keys for US

IME Keys for US は、US キーボードで日本語入力を使う人のための小さな Windows 常駐アプリです。

左 Alt / 右 Alt を IME の明示的な OFF / ON キーとして使います。IME トグルではありません。Word、ブラウザー、VS Code、ターミナルなどを行き来するときに、「いま日本語入力なのか英字入力なのか」を覚えておく負担を減らすことを目的にしています。

このプロジェクトはクリーンなネイティブ実装です。AutoHotkey は含みません。`alt-ime-ahk` などのコードもコピーしていません。

## 動作

| 入力 | 動作 |
|---|---|
| 左 Alt を 1 回押す | `DoubleTapMs` 後に IME OFF |
| 右 Alt を 1 回押す | `DoubleTapMs` 後に IME ON |
| 左 Alt を素早く2度押す | 通常の単独の左 Alt  |
| 右 Alt を素早く2度押す | 通常の単独の右 Alt  |
| 右 Alt を押したまま左 Alt | 通常の単独の左 Alt |
| 左 Alt を押したまま右 Alt | 通常の単独の右 Alt |
| 左 Alt + 他のキー | 通常の左 Alt + 他のキー |
| 右 Alt + 他のキー | 通常の右 Alt + 他のキー |

2 回目の同じ Alt が時間内に押された時点で、「IME ではなく通常の Alt を使いたい」と判断します。2 回目をすぐ離せば単独 Alt として動き、押したまま別のキーを押せば通常の Alt+キーとして動きます。

## 設定

設定ファイル:

```text
%APPDATA%\ImeKeysForUS\settings.ini
```

例:

```ini
[Keyboard]
DoubleTapMs=100
```

既定値は `100 ms` です。指定できる値は `100-500 ms` に丸められます。短い値ほど IME 切り替えの遅延が小さくなり、長い値ほど 2 回押しがしやすくなります。v0.1.0 では、設定変更後にアプリの再起動が必要です。

## 自動起動

MSI インストール時には自動起動を有効にしません。

現在のユーザーで自動起動したい場合は、タスクトレイメニューから `Start at sign-in` を有効にしてください。

コマンドでも設定できます。

```powershell
"C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe" --enable-startup
```

解除:

```powershell
"C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe" --disable-startup
```

## 既知の制限

- IME 制御は IMM ベースです。IME やアプリの組み合わせによっては完全に動かない場合があります。
- TSF 対応はまだありません。
- 管理者権限で起動したアプリ上で使うには、将来的に UIAccess 署名ビルドの検証が必要です。
- CI で作られる成果物は unsigned の場合があります。
- Microsoft Store 配布は v0.1.0 の対象外です。
- winget 提出は準備のみで、自動では行いません。
- Right Alt は、AltGr として扱われるキーボードレイアウトでは挙動が異なる可能性があります。
- 設定 UI はまだありません。

## English

English README: [README.en.md](README.en.md)

## 開発者向け情報

ビルド、署名、UIAccess、リリース手順は [docs/development.md](docs/development.md) を見てください。

手動テスト項目は [docs/manual-test-checklist.md](docs/manual-test-checklist.md) にあります。

## ライセンス

MIT License. See [LICENSE](LICENSE).
