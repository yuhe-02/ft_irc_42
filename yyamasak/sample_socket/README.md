## IRCサーバーとクライアント (Irssi) の立ち上げガイド

このREADMEは、IRCサーバーを立ち上げ、Irssiクライアントを使用して接続する方法をステップバイステップで解説します。

---

## 🚀 IRCサーバーの立ち上げ

### ✅ サーバーの起動方法
1. `sample_socket` ディレクトリへ移動します。
2. 以下のコマンドを実行してサーバーを起動します。

```bash
bash test.sh  # 必要な準備を実行
```

3. サーバーを起動します。

```bash
./irc_serv ${port} ${password}
```

- `${port}`: サーバーが待ち受けるポート番号  
- `${password}`: クライアントが接続時に使用するパスワード  

---

## 💻 IRCクライアント (Irssi) のセットアップ

### 🔗 参考サイト
[Irssi公式サイト](https://irssi.org/)

### 📥 クライアントのソースコード取得
```bash
# クライアント用ディレクトリを作成
mkdir irssi && cd irssi

# Irssiをダウンロード
wget https://codeberg.org/irssi/irssi/releases/download/1.4.5/irssi-1.4.5.tar.xz

# アーカイブを展開
tar xJf irssi-*.tar.xz
cd irssi-*
```

---

## 🛠️ ビルド環境の構築

### ⚠️ 事前準備
Ubuntu環境では以下のコマンドでビルドツールをインストールします。
```bash
sudo apt update
sudo apt install -y ninja-build meson
```

### 🔨 ビルド手順
```bash
# ビルドディレクトリ作成と初期化
meson Build  # "Build" または "build" の可能性あり

# ビルド実行
ninja -C Build 

# インストール
sudo ninja -C Build install
```
> 📝 **注意:** ディレクトリ名が `Build` ではなく `build` の場合があります。

---

## 🚀 クライアントの起動
```bash
irssi
```
> 📢 **注意:** ビルドしたディレクトリ内で実行してください。

---

## 📡 使用方法 (Usage)

### 🌐 接続方法
```irc
/connect ${ip_address} ${port}
```
- `${ip_address}`: サーバーのIPアドレス（通常は `127.0.0.1`）  
- `${port}`: サーバー起動時に指定したポート番号  

### 💬 メッセージ送信
```irc
/msg ${channel_name} ${message}
```
- `${channel_name}`: メッセージを送りたいチャンネル名  
- `${message}`: 送信するメッセージ  

---

## 🛡️ トラブルシューティング

### 🚫 よくある問題と解決策
- **`irssi: command not found` エラー**
  - インストールが完了しているか確認。
  - `sudo ninja -C Build install` を再実行。

- **接続できない場合**
  - サーバーが正しく起動しているか確認。
  - ポート番号やパスワードが一致しているか確認。
  - ファイアウォールやネットワーク設定を確認。

- **ビルドエラー時**
  - `ninja` と `meson` のインストール状況を確認。
  - 依存関係が不足していないか確認。

---