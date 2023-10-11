# include <Siv3D.hpp> // OpenSiv3D v0.6.11

// 共有するデータ
struct GameData
{
	bool win1 = false;
	bool win2 = false;
	bool even = false;
};

using App = SceneManager<String,GameData>;

// タイトルシーン
class Title : public App::Scene
{
public:

	Title(const InitData& init)
		: IScene{ init }
	{
	}

	~Title()
	{
	}

	void update() override
	{
		// 左クリックで
		if (MouseL.down())
		{
			// ゲームシーンに遷移
			changeScene(U"Game");
		}
	}

	void draw() const override
	{
		Scene::SetBackground(ColorF{ 0.3, 0.4, 0.5 });
		FontAsset(U"TitleFont")(U"○×ゲーム").drawAt(400, 200);
		FontAsset(U"TitleFont")(U"左クリックしてね").drawAt(400, 500);
	}
};

// ゲームシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init)
		: IScene{ init }
	{
		///変数
		/// 基本サイズ 50 のフォントを作成
		font = Font(50);

		count = 0;///手数カウント
		uteru = true;///着手可能か
		//win1 = false;//先手が勝ちか
		//win2 = false;//後手が勝ちか

		// ○×の描画判定
		maru.resize(9);
		batsu.resize(9);
		// 場所にすでに描画されているか
		pos.resize(9);
	}

	~Game()
	{
	}
	
	void update() override
	{
		if (MouseL.down()) {///左クリックされた
			if (count % 2 == 0 && uteru) {///先手なら
				for (int32 i = 0; i < 9; i++) {
					if ((250 + i % 3 * 100 < Cursor::Pos().x && Cursor::Pos().x < 350 + i % 3 * 100) && (150 + i / 3 * 100 < Cursor::Pos().y && Cursor::Pos().y < 250 + i / 3 * 100) && !pos[i]) {///場所判定
						maru[i] = true;
						pos[i] = true;
						count += 1;
					}
				}
			}
			else if (count % 2 != 0 && uteru) {///後手なら
				for (int32 i = 0; i < 9; i++) {
					if ((250 + i % 3 * 100 < Cursor::Pos().x && Cursor::Pos().x < 350 + i % 3 * 100) && (150 + i / 3 * 100 < Cursor::Pos().y && Cursor::Pos().y < 250 + i / 3 * 100) && !pos[i]) {///場所判定
						batsu[i] = true;
						pos[i] = true;
						count += 1;
					}
				}
			}

			///勝敗判定(先手勝ち)
			for (int32 i = 0; i < 3; i++) {
				if ((maru[i * 3] && maru[i * 3 + 1] && maru[i * 3 + 2])) {
					getData().win1 = true;
					uteru = false;///横判定
				}
			}

			for (int32 i = 0; i < 3; i++) {
				if ((maru[i] && maru[i + 3] && maru[i + 6])) {
					getData().win1 = true;
					uteru = false;///縦判定
				}
			}
			///ななめ判定
			if ((maru[0] && maru[4] && maru[8])) {
				getData().win1 = true;
				uteru = false;
			}
			if ((maru[2] && maru[4] && maru[6])) {
				getData().win1 = true;
				uteru = false;
			}

			///勝敗判定(後手勝ち)
			for (int32 i = 0; i < 3; i++) {
				if ((batsu[i * 3] && batsu[i * 3 + 1] && batsu[i * 3 + 2])) {
					getData().win2 = true;
					uteru = false;///横判定
				}
			}

			for (int32 i = 0; i < 3; i++) {
				if ((batsu[i] && batsu[i + 3] && batsu[i + 6])) {
					getData().win2 = true;
					uteru = false;///縦判定
				}
			}

			///ななめ判定
			if ((batsu[0] && batsu[4] && batsu[8])) {
				getData().win2 = true;
				uteru = false;
			}
			if ((batsu[2] && batsu[4] && batsu[6])) {
				getData().win2 = true;
				uteru = false;
			}
		}

		///引き分けなら
		if (((getData().win1 == false) && (getData().win2 == false)) && (count >= 9)) {
			getData().even = true;
		}

		if ((getData().win1 == true) || (getData().win2 == true) || (getData().even == true)) {
			changeScene(U"End");
		}
	}

	void draw() const override
	{
		// 背景の色を設定する | Set the background color
		Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

		///盤を作る
		Rect{ 250,150,5,300 }.draw();
		Rect{ 550,150,5,305 }.draw();
		Rect{ 250,150,300,5 }.draw();
		Rect{ 250,450,300,5 }.draw();
		Rect{ 250,250,300,5 }.draw();
		Rect{ 250,350,300,5 }.draw();
		Rect{ 350,150,5,300 }.draw();
		Rect{ 450,150,5,300 }.draw();

		///○×描画
		for (int32 i = 0; i < 9; i++) {
			if (maru[i]) Circle{ 300 + (i % 3 * 100), 205 + (i / 3 * 100), 40 }.drawArc(0_deg, 360_deg, 5, 0, Palette::Red);///押されてる場所に○描画
		}

		for (int32 i = 0; i < 9; i++) {
			if (batsu[i]) Shape2D::Cross(60, 5, Vec2{ 300 + (i % 3 * 100), 205 + (i / 3 * 100) }).draw(Palette::Blue);///×描画
		}

		/*///勝敗がついているなら
		if (win1 == true) {
			font(U"First Win").draw(500, 500);
		}

		if (win2 == true) {
			font(U"Second Win").draw(500, 500);
		}*/
	}

private:
	// メンバ変数として定義
	int32 count;  // 手数カウント
	bool uteru;   // 着手可能か
	//bool win1;    // 先手が勝ちか
	//bool win2;    // 後手が勝ちか
	Font font; // サイズを 50 に設定
	Array<bool> maru;
	Array<bool> batsu;
	Array<bool> pos;
};

// エンドシーン
class End : public App::Scene
{
public:

	// コンストラクタ（必ず実装）
	End(const InitData& init)
		: IScene{ init }
	{
		/// 基本サイズ 50 のフォントを作成
		font = Font(100);
	}

	~End()
	{
		getData().win1 = false;
		getData().win2 = false;
		getData().even = false;
	}


	// 更新関数（オプション）
	void update() override
	{
		if (MouseL.down())
		{
			// タイトルシーンに遷移
			changeScene(U"Title");
		}
	}

	// 描画関数（オプション）
	void draw() const override
	{
		Scene::SetBackground(ColorF{ 0.3, 0.4, 0.5 });

		if (getData().win1 == true) {
			font(U"○の勝ち!").draw(200, 200);
		}

		if (getData().win2 == true) {
			font(U"×の勝ち!").draw(200, 200);
		}
		if (getData().even == true) {
			font(U"引き分け").draw(200, 200);
		}
	}

private:
	Font font; // サイズを 50 に設定
};

void Main()
{
	FontAsset::Register(U"TitleFont", 60, Typeface::Heavy);

	// シーンマネージャーを作成
	App manager;
	manager.add<Title>(U"Title");
	manager.add<Game>(U"Game");
	manager.add<End>(U"End");

	// "Title" シーンから開始
	manager.init(U"Title");

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}

//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
