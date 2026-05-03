# IME Keys for US

IME Keys for US は、US キーボードで日本語 IME を使う人のための小さな Windows 常駐アプリです。

左 Alt / 右 Alt を IME の明示的な OFF / ON キーとして使います。IME トグルではありません。Word、ブラウザー、VS Code、ターミナルなどを行き来するときに、「いま日本語入力なのか英字入力なのか」を覚えておく負担を減らすことを目的にしています。

このプロジェクトはクリーンなネイティブ実装です。AutoHotkey は含みません。`alt-ime-ahk` などのコードもコピーしていません。

## このアプリは何をするものか

IME Keys for US は、US キーボードで日本語 IME を使う人のための Windows 常駐アプリです。

テキストエディタやキーボード配列ではありません。起動してもメインウィンドウは表示されず、通知領域 / タスクトレイに常駐します。

目的は、IME の「トグル」操作を避け、英字入力と日本語入力を明示的に切り替えられるようにすることです。

- 左 Alt: IME OFF / 英字入力側へ
- 右 Alt: IME ON / 日本語入力側へ

アプリや入力欄を行き来していると、現在の IME 状態を覚えておく必要があります。このアプリは、英字入力にしたいときは左 Alt、日本語入力にしたいときは右 Alt、という操作にすることで、その負担を減らすことを目的にしています。

## 起動後に表示されるもの

正常に起動すると、Windows の通知領域 / タスクトレイにアイコンが表示されます。

メインウィンドウは表示されません。

タスクトレイアイコンを右クリックすると、次のメニューが表示されます。

- Start at sign-in
- Exit

アイコンが見えない場合は、Windows の通知領域で隠れているアイコンも確認してください。

ただし、タスクトレイアイコンがまったく表示されず、Alt キーにも反応しない場合は正常ではありません。v0.1.3 以降では、起動時の状態を次のログに記録します。

```text
%LOCALAPPDATA%\ImeKeysForUS\ime-keys-for-us.log
```

致命的な初期化失敗が起きた場合は、メッセージボックスも表示します。

## 日本語 IME がない環境での見え方

このアプリの主な機能は、日本語 IME の ON / OFF を明示的に切り替えることです。

そのため、日本語 IME がインストールされていない環境や、日本語 IME を使っていない入力状態では、左 Alt / 右 Alt を押しても見た目の変化が分かりにくい場合があります。

それでも、アプリが正常に起動していればタスクトレイアイコンは表示されます。

タスクトレイアイコンが表示されない場合は、IME の有無とは別の起動問題として扱ってください。

## 動作確認の例

日本語 IME が入っている環境では、次のように確認できます。

1. IME Keys for US を起動する
2. タスクトレイにアイコンが表示されることを確認する
3. メモ帳などの入力欄を開く
4. Microsoft IME などの日本語 IME を使える状態にする
5. 右 Alt を押して、日本語入力側になることを確認する
6. 左 Alt を押して、英字入力側になることを確認する
7. タスクトレイアイコンを右クリックし、Exit で終了できることを確認する

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

既定値は `100 ms` です。`DoubleTapMs` は正の整数ミリ秒として読み込まれます。`0`、負数、非数値、安全に扱えないほど大きい値は既定値になります。短い値ほど IME 切り替えの遅延が小さくなり、長い値ほど 2 回押しがしやすくなります。v0.1.x では、設定変更後にアプリの再起動が必要です。

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

## プライバシー

IME Keys for US は、テレメトリを収集しません。キー入力内容を記録しません。外部ネットワークへデータを送信しません。

低レベルキーボードフックは、左 Alt / 右 Alt の操作をローカルで判定し、IME の ON / OFF を要求するためだけに使います。

起動診断ログは、ローカル環境の次の場所にだけ保存されます。

```text
%LOCALAPPDATA%\ImeKeysForUS\ime-keys-for-us.log
```

このログは自動送信されません。

## 署名について

配布用の `exe` と `MSI` は、リリース時にローカルの `CN=tsuchim` コード署名証明書で Authenticode 署名します。

これは個人開発の OSS として、公開 CA のコード署名証明書ではありません。そのため、環境によっては Windows SmartScreen や発行元の警告が表示される場合があります。

それでも、配布 artifact が署名後に改ざんされていないことを確認できるようにするため、署名付き artifact を公開しています。winget では manifest の SHA256 でも installer が検証されます。

SignPath Foundation に申請済みで、現在返答待ちです。受理された場合、SignPath Foundation による署名は署名 workflow 統合後のリリースで導入する予定です。

コード署名ポリシー: [docs/code-signing-policy.md](docs/code-signing-policy.md)

## 既知の制限

- IME 制御は IMM ベースです。IME やアプリの組み合わせによっては完全に動かない場合があります。
- TSF 対応はまだありません。
- 管理者権限で起動したアプリ上で使うには、将来的に UIAccess 署名ビルドの検証が必要です。
- CI で作られる成果物は unsigned の場合があります。v0.1.6 の推奨配布物はローカルで署名した artifact です。
- Microsoft Store 配布は v0.1.x の対象外です。
- Right Alt は、AltGr として扱われるキーボードレイアウトでは挙動が異なる可能性があります。
- 設定 UI はまだありません。

## English

English README: [README.en.md](README.en.md)

## 開発者向け情報

ビルド、署名、UIAccess、リリース手順は [docs/development.md](docs/development.md) を見てください。

手動テスト項目は [docs/manual-test-checklist.md](docs/manual-test-checklist.md) にあります。

## ライセンス

MIT License. See [LICENSE](LICENSE).
