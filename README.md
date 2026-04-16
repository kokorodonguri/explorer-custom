# ExplorerImageTool

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows_11-lightgrey.svg)
![Architecture](https://img.shields.io/badge/arch-x64_%7C_ARM64-orange.svg)

ExplorerImageTool は、Windows 11 のエクスプローラーに好きな画像（キャラクターの立ち絵など）を透過表示させるための軽量なオーバーレイツールです。

エクスプローラー内部のプログラムを改変しない「オーバーレイ方式」を採用しているため、Windowsのアップデートで壊れにくく、最新のWindows 11環境（x64 / ARM64）でも安全かつ安定して動作します。

## ✨ 特徴 (Features)

* **安全なオーバーレイ方式:** エクスプローラー自体をハッキングしないため、Windows Updateの影響を受けにくく安全です。
* **神追従（ゼロ遅延）:** Windows OSのイベントフック機能（`SetWinEventHook`）を使用し、エクスプローラーを激しく動かしても画像がピタッと吸い付きます。
* **自動ファイルリスト検知:** 詳細ウィンドウやナビゲーションペインを避け、メインのファイルリストエリアにのみ画像を配置します。
* **ランダム画像切り替え:** 複数の画像を登録でき、エクスプローラーをアクティブにするたびにランダムに画像が切り替わります。
* **ARM64ネイティブ対応:** x64環境はもちろん、Snapdragon等を搭載したARM64版Windowsでもネイティブで軽快に動作します。

## 🚀 使い方 (Usage)

### 1. 準備
1.  リリース（Releases）から最新の `ExplorerImageTool.exe` をダウンロードします。
2.  任意のフォルダに配置します。

### 2. 画像の追加
1.  `ExplorerImageTool.exe` を一度起動します。
2.  起動すると、`.exe` と同じ階層に自動で **`img`** というフォルダが作成されます。
3.  作成された `img` フォルダの中に、表示させたい画像ファイル（`.png`, `.jpg`, `.jpeg`）を入れてください。
    * ※背景が透過されたPNG画像（立ち絵など）を推奨します。

### 3. 実行と操作
* **起動:** `ExplorerImageTool.exe` をダブルクリックして実行します。
* **表示:** エクスプローラーのウィンドウを選択（アクティブに）すると、右下に画像が表示されます。
* **終了:** エクスプローラーをアクティブにした状態で、キーボードの `Ctrl` + `Shift` + `Q` を押すとツールが終了します。

## ⚙️ 動作環境 (Requirements)

* **OS:** Windows 11
* **Architecture:** x64 または ARM64

## 🛠️ ビルド方法 (Build from source)

開発環境として **Visual Studio 2022 (C++)** が必要です。

1.  このリポジトリをクローンします。
2.  Visual Studioで新規の「Windows デスクトップ アプリケーション (C++)」プロジェクトを作成します。
3.  `main.cpp` にソースコードを貼り付けます。
4.  プロジェクトのプロパティから、ターゲットアーキテクチャ（x64 または ARM64）を選択します。
5.  Releaseモードでビルドします。

## ⚠️ 注意事項 (Notes)

* このツールは「現在一番手前にあるエクスプローラー」にのみ画像を重ねます。裏にあるエクスプローラーには表示されません。
* 画像の不透明度や表示位置を変更したい場合は、ソースコード（`main.cpp` 内の `DrawOverlay` 関数など）の数値を変更して再ビルドしてください。

## 📄 ライセンス (License)

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
