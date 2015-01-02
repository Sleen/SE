#include "SE.h"

#ifdef _DEBUG
#   define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif

#include "ImageButton.h"
#include <random>

#include <thread>

#include "graphics\Shader.h"
#include "json11.hpp"
#include "Type.h"

using namespace json11;

USING_SE

#include "View.h"
using namespace SEView;

class MyView : public FrameLayout{
    DECL_CLASS(MyView);
private:
    bool checked = false;
    string text;
    float fontSize;

    shared_ptr<SEView::Label> label;
    shared_ptr<View> line;

public:
    MyView(){
        label = shared_ptr<SEView::Label>(new SEView::Label);
        label->SetColor(Color::Black);

        line = shared_ptr<View>(new View());
        line->SetBackColor(Color::LightBlue);

        Add(label, shared_ptr<LayoutParams>(new LayoutParams(MATCH_PARENT, MATCH_PARENT)));
        Add(line, shared_ptr<LayoutParams>(new LayoutParams(MATCH_PARENT, Unit::ToPixel(3, Unit::UNIT_DP), BOTTOM)));
    }

    virtual void OnDrawFore(DrawEventArgs e) override{
        label->SetText(text);
        label->SetFont(FontManager::CreateFont("", fontSize).get());
        line->SetVisibled(checked);
        FrameLayout::OnDrawFore(e);
    }
};

IMPL_CLASS_WITH_FIELDS(MyView, FrameLayout,
    FIELD(MyView, checked, Bool),
    FIELD(MyView, text, String),
    FIELD(MyView, fontSize, Float)
    );

#define XYZ(v) (v).X(), (v).Y(), (v).Z()
class ModelViewer : public View{
    DECL_CLASS(ModelViewer);
protected:
    shared_ptr<Model> model;
    Vector3 origin, translate;
    Matrix4 rotate = Matrix4::Identity();
    shared_ptr<RenderableTexture> buffer;
    Vector2 down;
    bool isCtrl = false;

public:
    virtual void OnMeasure(float width, float height, int widthMode, int heightMode) override{
        View::OnMeasure(width, height, widthMode, heightMode);
        int w = (int)(measuredWidth - totalPadding.GetWidth());
        int h = (int)(measuredHeight - totalPadding.GetHeight());
        if (w <= 0 || h <= 0) return;
        if (buffer == nullptr || buffer->GetTexture()->GetWidth() != w || buffer->GetTexture()->GetHeight() != h){
            buffer = make_shared<RenderableTexture>(measuredWidth - totalPadding.GetWidth(), measuredHeight - totalPadding.GetHeight(), true);
        }
    }

    virtual void OnKeyDown(KeyEventArgs e) override{
        if (e.KeyCode == VK_CONTROL){
            isCtrl = true;
        }
    }

    virtual void OnKeyUp(KeyEventArgs e) override{
        if (e.KeyCode == VK_CONTROL){
            isCtrl = false;
        }
    }

    virtual void OnMouseDown(MouseEventArgs e) override{
        down = e.Location();
    }

    virtual void OnMouseMove(MouseEventArgs e) override{
        switch (e.Button){
        case MouseButton::Left:
        {
            if (isCtrl){
                down = e.Location();
                model->ClearLights();
                model->AddLight(LightSource::DirectionalLight(Vector3(down.X() - measuredWidth / 2, -(down.Y() - measuredHeight / 2), 0), Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1)));
            }
            else{
                Vector2 delta = e.Location() - down;
                translate += Vector3(delta.X(), -delta.Y(), 0) / buffer->GetTexture()->GetWidth() * 2;
                down = e.Location();
            }
            break;
        }
        case MouseButton::Right:
            rotate = rotate * RotateMatrix(-(e.Y - down.Y()), 1, 0, 0);
            rotate = rotate * RotateMatrix(-(e.X - down.X()), 0, 1, 0);
            down = e.Location();
            break;
        }
    }

    virtual bool HasMouseEvent() override{
        return true;
    }

    virtual void OnDrawFore(DrawEventArgs e) override{
        if (model == nullptr){
            return;
        }
        
        buffer->Begin();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        MatrixMode(PROJECTION_MATRIX);
        LoadIdentity();
        
        float r = (measuredHeight - totalPadding.GetHeight()) / (measuredWidth - totalPadding.GetWidth());
        float an = 120;
        Frustum(-1, 1, r, -r, 1/tanf(an*M_PI/360), 10);

        MatrixMode(MODELVIEW_MATRIX);
        LoadIdentity();

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        Translate(translate.X(), translate.Y(), translate.Z());
        Translate(origin.X(), origin.Y(), origin.Z());
        Transform::Instance()->MultiplyMatrix(rotate);
        Translate(-origin.X(), -origin.Y(), -origin.Z());

        model->Draw();

        Translate(origin.X(), origin.Y(), origin.Z());
        Graphics::UseShader();
        e.GetGraphics().DrawLine(Color::Red, Vector3(), Vector3(3, 0, 0));
        e.GetGraphics().DrawLine(Color::Green, Vector3(), Vector3(0, 3, 0));
        e.GetGraphics().DrawLine(Color::Blue, Vector3(), Vector3(0, 0, 3));

        glEnable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);
        buffer->End();
        Graphics::UseShader();
        e.GetGraphics().DrawImage(*buffer->GetTexture(), Rect(0, 0, buffer->GetTexture()->GetWidth(), buffer->GetTexture()->GetHeight()));
    }

    void SetModel(shared_ptr<Model>model, bool autoOrigin = true){
        this->model = model;
        translate = Vector3(0, 0, -2);
        if (autoOrigin){
            auto& vs = model->Vertices();
            float xmin = vs[0].X(), xmax = vs[0].X(), ymin = vs[0].Y(), ymax = vs[0].Y(), zmin = vs[0].Z(), zmax = vs[0].Z();
            for (auto& v : vs){
                if (v.X() < xmin) xmin = v.X();
                if (v.X() > xmax) xmax = v.X();
                if (v.Y() < ymin) ymin = v.Y();
                if (v.Y() > ymax) ymax = v.Y();
                if (v.Z() < zmin) zmin = v.Z();
                if (v.Z() > zmax) zmax = v.Z();
            }
            origin = Vector3((xmax + xmin) / 2, (ymax + ymin) / 2, (zmax + zmin) / 2);
            translate.X() = -origin.X();
            translate.Y() = -origin.Y();
        }
    }
};

IMPL_CLASS(ModelViewer, View);

class MyWindow : public SE::Window{
protected:
    shared_ptr<View> contentView;
    shared_ptr<Model> teapot;

    void test1(){
        shared_ptr<Texture> zazaka(new Texture("drawable/zazaka.png"));
        shared_ptr<Texture> divider(new Texture("drawable/divider.png"));
        shared_ptr<NinePatchTexture> page(new NinePatchTexture("drawable/page.9.png"));
        shared_ptr<NinePatchTexture> btn(new NinePatchTexture("drawable/btn.9.png"));
        shared_ptr<NinePatchTexture> bubble(new NinePatchTexture("drawable/bubble.9.png"));

        shared_ptr<LinearLayout> l(new LinearLayout);

        l->SetPadding(Padding(10));
        //l->SetBackground(shared_ptr<ColorDrawable>(new ColorDrawable(Color::Pink)));
        l->SetBackground(shared_ptr<NinePatchTextureDrawable>(new NinePatchTextureDrawable(page)));

        shared_ptr<LinearLayout> top(new LinearLayout);
        top->SetBackColor(Color::White);
        top->SetOrientation(HORIZONTAL);
        shared_ptr<ImageView> b1(new ImageView);
        b1->SetBackColor(Color::Transparent);
        b1->SetTexture(zazaka);
        top->Add(b1, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(30, 30)));

        shared_ptr<SEView::Label> title(new SEView::Label);
        title->SetText("这是标题");
        title->SetPadding(Padding(5));
        title->SetBackColor(Color::Transparent);
        top->Add(title, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, MATCH_PARENT, 1)));

        shared_ptr<ImageView> b2(new ImageView);
        b2->SetBackColor(Color::Transparent);
        b2->SetTexture(zazaka);
        top->Add(b2, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(30, 30)));

        l->Add(top, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, WRAP_CONTENT)));

        shared_ptr<LinearLayout> list(new LinearLayout);
        list->SetBackground(shared_ptr<RectDrawable>(new RectDrawable(Color::White, Color::Gray, 1, 5)));
        list->SetPadding(Padding(1));
        list->SetMargin(Padding(0, 10, 0, 0));
        //list->SetDividerDrawable(shared_ptr<LineDrawable>(new LineDrawable(Color::Gray)));
        list->SetDividerDrawable(shared_ptr<TextureDrawable>(new TextureDrawable(divider)));
        shared_ptr<SEView::Label> lb1(new SEView::Label);
        shared_ptr<SEView::Label> lb2(new SEView::Label);
        shared_ptr<SEView::Label> lb3(new SEView::Label);
        lb1->SetText(R"(void glStencilFunc(
  GLenum func,
  GLint ref,
  GLuint mask
);)");
        lb2->SetText("OpenGL is a cross-platform standard 2D and 3D graphics API.");
        lb3->SetText("测试文字3");
        lb1->SetFontFlag(FontFlag::FontFlag_Left);
        lb1->SetPadding(Padding(4));
        lb2->SetPadding(Padding(4));
        lb3->SetPadding(Padding(4));
        list->Add(lb1, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(WRAP_CONTENT, WRAP_CONTENT, 0, CENTER)));
        list->Add(lb2, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(WRAP_CONTENT, WRAP_CONTENT)));
        list->Add(lb3, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, WRAP_CONTENT)));

        l->Add(list, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(WRAP_CONTENT, WRAP_CONTENT)));

        shared_ptr<LinearLayout> bs(new LinearLayout);
        bs->SetOrientation(HORIZONTAL);
        bs->SetMargin(Padding(0, 10, 0, 0));
        bs->SetBackColor(Color::Transparent);

        //shared_ptr<Drawable> btn_back = shared_ptr<TextureDrawable>(new TextureDrawable(button_back
        shared_ptr<Drawable> btn_back = shared_ptr<RectDrawable>(new RectDrawable(0xffff4500, 0, 0, 3));

        shared_ptr<View> v1(new View);
        shared_ptr<View> v2(new View);
        shared_ptr<View> v3(new View);
        v1->SetBackColor(Color::Transparent);
        v2->SetBackColor(Color::Transparent);
        v3->SetBackColor(Color::Transparent);
        shared_ptr<SEView::Label> bt1(new SEView::Label);
        shared_ptr<SEView::Label> bt2(new SEView::Label);
        bt1->SetText("按钮一");
        bt2->SetText("按钮二");
        bt1->SetColor(Color::White);
        bt2->SetColor(Color::Black);
        bt1->SetPadding(Padding(8));
        //bt2->SetPadding(Padding(8));
        bt1->SetBackground(btn_back);
        bt2->SetBackground(shared_ptr<NinePatchTextureDrawable>(new NinePatchTextureDrawable(btn)));

        bt1->MouseEnter += [bt1](View& v) { bt1->SetBackground(shared_ptr<RectDrawable>(new RectDrawable(0xaaff4500, 0, 0, 3))); };
        bt1->MouseLeave += [bt1](View& v) { bt1->SetBackground(shared_ptr<RectDrawable>(new RectDrawable(0xffff4500, 0, 0, 3))); bt1->SetText("按钮一"); };
        bt1->Click += [bt1](View& v) { bt1->SetText("Clicked!"); };

        bs->Add(v1, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(20, MATCH_PARENT)));
        bs->Add(bt1, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, WRAP_CONTENT, 1, CENTER)));
        bs->Add(v2, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(30, MATCH_PARENT)));
        bs->Add(bt2, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, WRAP_CONTENT, 1, CENTER)));
        bs->Add(v3, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(20, MATCH_PARENT)));

        l->Add(bs, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, WRAP_CONTENT)));

        shared_ptr<LinearLayout> images(new LinearLayout);
        images->SetPadding(Padding(5, 5, 0, 5));
        images->SetMargin(Padding(0, 10, 0, 0));
        images->SetOrientation(HORIZONTAL);

        for (int i = 0; i < 6; i++){
            shared_ptr<ImageView> image(new ImageView);
            image->SetMargin(Padding(0, 0, 5, 0));
            image->SetTexture(zazaka, SizeMode::None);
            image->SetBackground(shared_ptr<NinePatchTextureDrawable>(new NinePatchTextureDrawable(bubble)));
            images->Add(image, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(WRAP_CONTENT, WRAP_CONTENT)));
        }

        l->Add(images, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(WRAP_CONTENT, WRAP_CONTENT)));

        shared_ptr<SEView::Label> bb(new SEView::Label);
        bb->SetText("这是一个聊天气泡\n换行测试");
        bb->SetBackground(shared_ptr<NinePatchTextureDrawable>(new NinePatchTextureDrawable(bubble)));
        l->Add(bb, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(WRAP_CONTENT, WRAP_CONTENT)));

        l->Add(shared_ptr<SEView::Slider>(new SEView::Slider), shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, WRAP_CONTENT)));

        shared_ptr<View> content(new View);
        content->SetMargin(Padding(0, 10, 0, 0));
        content->SetBackground(shared_ptr<RectDrawable>(new RectDrawable(Color::White, Color::Gray, 1, 5, /*0011*/3)));
        l->Add(content, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, MATCH_PARENT, 1)));

        shared_ptr<View> footer(new View);
        footer->SetBackground(shared_ptr<RectDrawable>(new RectDrawable(Color::White, Color::Gray, 1, 5, /*1100*/12)));
        l->Add(footer, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, 30)));

        auto size = this->GetClientSize();
        l->OnMeasure(size.X(), size.Y(), EXACTLY, EXACTLY);
        l->OnLayout();

        contentView = l;

        //        new std::thread([lb2]
        //        {
        //            string s = "test text. test text. test text. test text. test text.";
        //            int i = 0, n = s.length();
        //            while (++i){
        //                int t = n - abs(i % (2 * n) - n);
        //                lb2->SetText(s.substr(0, t));
        //#ifdef WINDOWS
        //                Sleep(30);
        //#else
        //                usleep(30000);
        //#endif
        //            }
        //        });
    }

    class Block : public SEView::Label{
    public:
        vector<Block*> around;
        bool isMine = false;
        bool opened = false;
        static shared_ptr<Drawable> btn_gray;
        static shared_ptr<Drawable> btn_red;

        void AddAround(Block* block){
            around.push_back(block);
            block->around.push_back(this);
        }

        int AroundMines(){
            int n = 0;
            for (auto b : around)
                n += b->isMine ? 1 : 0;
            return n;
        }

        void Open(){
            if (opened) return;

            SetEnabled(false);
            SetBackground(isMine ? btn_red : btn_gray);

            opened = true;
            int m = AroundMines();
            SetText(isMine ? "*" : m == 0 ? "" : toStr(m));
            if (!isMine && m == 0){
                for (auto b : around){
                    b->Open();
                }
            }
        }
    };

    class MyView1 : public LinearLayout{
    private:
        int x, y, mineCount;
        shared_ptr<Block>** blocks;
        shared_ptr<Drawable> btn;

    public:
        MyView1(int x = 10, int y = 10, int mineCount = 10){
            btn = Resources::LoadDrawable("drawable/btn");
            auto btn_gray = Resources::LoadDrawable("drawable/btn_gray");
            auto btn_red = Resources::LoadDrawable("drawable/btn_red");
            auto btn_pressed = Resources::LoadDrawable("drawable/btn_pressed");

            Block::btn_gray = btn_gray;
            Block::btn_red = btn_red;

            this->x = x;
            this->y = y;
            this->mineCount = mineCount;

            blocks = new shared_ptr<Block>*[x];
            for (int i = 0; i < x; i++){
                blocks[i] = new shared_ptr<Block>[y];
            }

            this->SetOrientation(HORIZONTAL);

            auto mouseDown = [btn_pressed](View& v, MouseEventArgs e){ v.SetBackground(btn_pressed); };
            auto mouseUp = [this](View& v, MouseEventArgs e){ v.SetBackground(btn); };
            auto click = [this, x, y, btn_gray](View& v) {
                Block& b = static_cast<Block&>(v);
                b.Open();
                if (b.isMine){
                    for (int i = 0; i < x; i++){
                        for (int j = 0; j < y; j++){
                            if (blocks[i][j]->isMine){
                                blocks[i][j]->Open();
                            }
                            else{
                                blocks[i][j]->SetEnabled(false);
                                //blocks[i][j]->SetBackground(btn_gray);
                            }
                        }
                    }
                }
            };

            for (int i = 0; i < x; i++){
                shared_ptr<LinearLayout> row(new LinearLayout());
                this->Add(row, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, MATCH_PARENT, 1)));
                for (int j = 0; j < y; j++){
                    auto b = shared_ptr<Block>(new Block());
                    b->SetBackground(btn);
                    b->MouseDown += mouseDown;
                    b->MouseUp += mouseUp;
                    b->Click += click;
                    blocks[i][j] = b;
                    row->Add(b, shared_ptr<LinearLayout::LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, MATCH_PARENT, 1)));
                    /*
                        oox
                        o*x
                        oxx
                        */
                    if (i - 1 >= 0 && j - 1 >= 0) b->AddAround(blocks[i - 1][j - 1].get());
                    if (j - 1 >= 0) b->AddAround(blocks[i][j - 1].get());
                    if (i - 1 >= 0) b->AddAround(blocks[i - 1][j].get());
                    if (i - 1 >= 0 && j + 1 < y) b->AddAround(blocks[i - 1][j + 1].get());
                }
            }

            NewGame();
        }

        void NewGame(){
            int mineCount = this->mineCount;

            for (int i = 0; i < x; i++){
                for (int j = 0; j < y; j++){
                    auto b = blocks[i][j];
                    b->SetText("");
                    b->SetEnabled(true);
                    b->SetBackground(btn);
                    b->opened = false;
                    b->isMine = false;
                }
            }

            mt19937 rnd(GetMicroSecond());

            while (mineCount){
                int i = rnd() % x;
                int j = rnd() % y;
                if (!blocks[i][j]->isMine){
                    blocks[i][j]->isMine = true;
                    mineCount--;
                }
            }


        }

        virtual void OnMeasure(float width, float height, int widthMode, int heightMode) override{
            float m = min(width, height);
            LinearLayout::OnMeasure(m, m, widthMode, heightMode);
        }

    };

    void test2(){
        shared_ptr<MyView1> view(new MyView1());
        shared_ptr<SEView::Label> label(new SEView::Label());
        shared_ptr<ViewGroup> layout(new LinearLayout());

        auto page = Resources::LoadDrawable("drawable/page");
        auto btn = Resources::LoadDrawable("drawable/btn");
        auto btn_pressed = Resources::LoadDrawable("drawable/btn_pressed");
        
        view->SetBackground(btn);
        label->SetBackground(btn);

        label->SetText("新游戏");
        label->SetColor(Color::Black);

        auto mouseDown = [btn_pressed](View& v, MouseEventArgs e){ v.SetBackground(btn_pressed); };
        auto mouseUp = [btn](View& v, MouseEventArgs e){ v.SetBackground(btn); };

        label->MouseDown += mouseDown;
        label->MouseUp += mouseUp;

        label->Click += [view](View& v){ view->NewGame(); };

        layout->Add(view, shared_ptr<LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, WRAP_CONTENT)));
        layout->Add(label, shared_ptr<LayoutParams>(new LinearLayout::LayoutParams(MATCH_PARENT, WRAP_CONTENT)));

        contentView = layout;
    }

    void test3(){
        contentView = Resources::LoadView("layout/scroll.json");

        Resources::FindViewById("btn")->Click += [this](View& v){
            static_pointer_cast<TextureDrawable>(contentView->GetBackground())->GetTexture()->SaveToBitmap()->Save("test.png");
            //new thread([this](){ static_pointer_cast<TextureDrawable>(contentView->GetBackground())->GetTexture()->SaveToBitmap()->Save("test.png"); });
        };

        //
        //auto label = make_shared<SEView::Label>();
        //label->SetText("Result     ");
        //label->SetColor(Color::Black);
        //label->SetFont(FontManager::CreateFont("", 20).get());
        //static_pointer_cast<ViewGroup>(contentView)->Add(label, make_shared<LinearLayout::LayoutParams>(WRAP_CONTENT, WRAP_CONTENT));
        //GLuint id = 2;
        ////glDeleteTextures(1, &id);

        //Resources::FindViewById("btn")->Click += [label](View& v){
        //    new thread([label, &v](){
        //        label->SetText("Running");
        //        v.SetEnabled(false);
        //        const int NUM = 100;
        //        int boxes[NUM];
        //        memset(boxes, 0, sizeof(int)*NUM);
        //        mt19937 rnd(GetMicroSecond());
        //        for (int i = 0; i < NUM; i++){
        //            int r;
        //            do{
        //                r = rnd() % NUM;
        //            } while (boxes[r] > 0);
        //            boxes[r] = i;
        //        }
        //        bool failed = false;
        //        for (int i = 0; i < NUM; i++){
        //            cout << "[" << i + 1 << "]  ";
        //            int now = i;
        //            for (int j = 0; j < 50; j++){
        //                if (j != 0) cout << "->";
        //                cout << boxes[now] + 1;
        //                if (boxes[now] == i) break;
        //                now = boxes[now];
        //            }
        //            cout << endl;
        //            if (boxes[now] != i) { failed = true; break; }
        //        }
        //        string result = failed ? "failed" : "succeed";
        //        label->SetText(result);
        //        v.SetEnabled(true);
        //    });
        //};
    }

    void test4(){
        auto view = make_shared<FrameLayout>();
        auto main = Resources::LoadView("layout/main.json");
        auto nowplay = Resources::LoadView("layout/play.json");
        nowplay->GetClass()->GetField("translateY")->Set<float>(nowplay.get(), GetClientHeight());
        nowplay->SetVisibled(false);
        view->Add(main, make_shared<FrameLayout::LayoutParams>(MATCH_PARENT, MATCH_PARENT));
        view->Add(nowplay, make_shared<FrameLayout::LayoutParams>(MATCH_PARENT, MATCH_PARENT));
        contentView = view;
        contentView->SetPadding(Padding(0, Unit::ToPixel(14, Unit::UNIT_DP), 0, 0));

        FileSystem::GetInstance().AddPath("E:\\Git\\SE\\res\\model");
        teapot = Model::Load("model/m.obj", 1.0f);
        teapot->AddLight(LightSource::DirectionalLight(Vector3(-1, 0, 0), Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1)));
        //teapot->MakeUVMap(this)->Save("uv.png");
        auto viewer = Resources::FindViewById("model1");
        static_pointer_cast<ModelViewer>(viewer)->SetModel(teapot);

        auto an = make_shared<Sequence>();
        an->Add(make_shared<ByAnimation<Vector2>>(viewer.get(), viewer->GetClass()->GetField("scale"), Vector2(1, 1)));
        an->Add(make_shared<ToAnimation<Vector2>>(viewer.get(), viewer->GetClass()->GetField("scale"), Vector2(1, 1)));
        an->Add(make_shared<Delay>(0.5f));
        an->Add(make_shared<ToAnimation<float>>(viewer.get(), viewer->GetClass()->GetField("rotate"), 360, 7));
        //an->Play();

        if (Resources::FindViewById("model2"))
        static_pointer_cast<ModelViewer>(Resources::FindViewById("model2"))->SetModel(Model::Load("model/teapot.obj", 1.0f / 30));

        //static_pointer_cast<ImageView>(Resources::FindViewById("add_icon"))->SetDrawable(make_shared<FrameDrawable>(Resources::LoadTexture("drawable/gem"), 30));

        auto btnMore = Resources::FindViewById("more");
        auto btnPlay = Resources::FindViewById("btn_play");
        auto btnNext = Resources::FindViewById("btn_next");
        auto btnList = Resources::FindViewById("btn_list");
        auto btnBottom = Resources::FindViewById("bottom");
        auto btnReturn = Resources::FindViewById("nowplay_return");

        auto list_layout = Resources::FindViewById("list_layout");
        auto list = Resources::FindViewById("list");

        auto curve = make_shared<BezierCurve>(0.5, 0, 0, 1);

        //list->GetClass()->GetField("rotate")->Set<float>(contentView.get(), 10);
        //list->GetClass()->GetField("scale")->Set<Vector2>(contentView.get(), Vector2(0.9f, 0.9f));

        list->GetClass()->GetField("scale")->Set<Vector2>(list.get(), Vector2(0.9f, 0.9f));
        list->GetClass()->GetField("alpha")->Set<float>(list.get(), 0);

        auto show_list = make_shared<Union>();
        //show_list->Add(make_shared<ToAnimation<Vector2>>(list.get(), View::clazz.GetField("scale"), Vector2(1, 1), 0.8f));
        show_list->Add(make_shared<ToAnimation<Vector2>>(list.get(), View::clazz.GetField("scale"), Vector2(1, 1), 0.3f));
        show_list->Add(make_shared<ToAnimation<float>>(list.get(), View::clazz.GetField("alpha"), 1, 0.3f));
        auto hide_list = make_shared<Union>();
        hide_list->Add(make_shared<ToAnimation<Vector2>>(list.get(), View::clazz.GetField("scale"), Vector2(0.9f, 0.9f), 0.3f));
        hide_list->Add(make_shared<ToAnimation<float>>(list.get(), View::clazz.GetField("alpha"), 0, 0.3f));
        //show_list->SetCurve(make_shared<FlexCurve>());
        show_list->SetCurve(curve);
        hide_list->SetCurve(curve);
        hide_list->End += [list_layout](Animation& a){
            list_layout->SetVisibled(false);
        };
        btnList->Click += [list, list_layout, show_list](View& v){
            list_layout->SetVisibled(true);
            show_list->Reset();
            show_list->Play();
        };
        list_layout->Click += [list, hide_list](View& v){
            hide_list->Reset();
            hide_list->Play();
        };

        auto trans = make_shared<Union>();
        trans->Add(make_shared<ByAnimation<float>>(nowplay.get(), View::clazz.GetField("translateY"), -GetClientHeight(), 0.6f));
        trans->Add(make_shared<ByAnimation<float>>(btnBottom.get(), View::clazz.GetField("translateY"), -GetClientHeight(), 0.6f));
        trans->Add(make_shared<ByAnimation<Vector2>>(main.get(), View::clazz.GetField("scale"), Vector2(-0.1f, -0.1f), 0.6f));
        auto trans2 = make_shared<Union>();
        trans2->Add(make_shared<ByAnimation<float>>(nowplay.get(), View::clazz.GetField("translateY"), GetClientHeight(), 0.6f));
        trans2->Add(make_shared<ByAnimation<float>>(btnBottom.get(), View::clazz.GetField("translateY"), GetClientHeight(), 0.6f));
        trans2->Add(make_shared<ByAnimation<Vector2>>(main.get(), View::clazz.GetField("scale"), Vector2(0.1f, 0.1f), 0.6f));
        trans->SetCurve(curve);
        trans2->SetCurve(curve);
        trans2->End += [main, nowplay](Animation& ani){ 
            nowplay->SetVisibled(false);
            nowplay->SetEnabled(true);
            main->SetEnabled(true);
        };
        trans->End += [main, nowplay](Animation& ani){
            main->SetVisibled(false);
            nowplay->SetEnabled(true);
            main->SetEnabled(true);
        };

        btnBottom->Click += [main, nowplay, trans](View& v){
            nowplay->SetVisibled(true);
            nowplay->SetEnabled(false);
            main->SetEnabled(false);
            trans->Reset();
            trans->Play();
        };
        btnReturn->Click += [main, nowplay, trans2](View& v){
            main->SetVisibled(true);
            nowplay->SetEnabled(false);
            main->SetEnabled(false);
            trans2->Reset();
            trans2->Play();
        };
    }

    void OnMouseMove(MouseEventArgs e) override{
        if (contentView){
            auto p = contentView->TransformPoint(e.Location());
            e.X = p.X();
            e.Y = p.Y();
            contentView->OnMouseMove(e);
        }
    }

    void OnMouseUp(MouseEventArgs e) override{
        if (contentView){
            auto p = contentView->TransformPoint(e.Location());
            e.X = p.X();
            e.Y = p.Y();
            contentView->OnMouseUp(e);
        }
    }

    void OnMouseDown(MouseEventArgs e) override{
        if (contentView){
            auto p = contentView->TransformPoint(e.Location());
            e.X = p.X();
            e.Y = p.Y();
            contentView->OnMouseDown(e);
        }
    }

    void OnKeyDown(KeyEventArgs e) override{
        if (contentView){
            contentView->OnKeyDown(e);
        }
    }

    void OnKeyUp(KeyEventArgs e) override{
        if (contentView){
            contentView->OnKeyUp(e);
        }
    }

    void OnLoad() override{
        DEBUG_LOG("OnLoad()\n");
        Window::OnLoad();
        FontManager::Init();

        alpha_buffer = make_shared<RenderableTexture>(GetClientWidth(), GetClientHeight(), true);

        DEBUG_LOG("screen dpi: %f\n", Unit::ScreenDpi());

        Control::SetDefaultFont(FontManager::GetDefaultFont().get());

        FileSystem::GetInstance().AddPath("E:\\Git\\SE\\res");

        test4();

        auto size = GetClientSize();
        contentView->Measure(size.X(), size.Y(), EXACTLY, EXACTLY);
        contentView->OnLayout();
    }
    
    void OnResize(const Vector2& size) override{
        Window::OnResize(size);
        if (contentView == nullptr) return;
        contentView->Measure(size.X(), size.Y(), EXACTLY, EXACTLY);
        contentView->OnLayout();
        alpha_buffer = make_shared<RenderableTexture>(GetClientWidth(), GetClientHeight(), true);
    }

    void OnUnload() override{
        DEBUG_LOG("OnUnload()\n");
        Window::OnUnload();
    }

    void OnDraw(DrawEventArgs e){
        AnimationManager::Update(GetDeltaTime());

        int64_t t = GetMicroSecond();

        Graphics g = e.GetGraphics();

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);
        glEnable(GL_STENCIL_TEST);

        glStencilFunc(GL_ALWAYS, 0, 0);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        if (contentView){
            auto size = this->GetClientSize();
            //contentView->Measure(size.X(), size.Y(), EXACTLY, EXACTLY);
            //contentView->OnLayout();
            g.Begin();
            g.Translate(contentView->GetMargin().GetLeft(), contentView->GetMargin().GetTop());
            contentView->OnDraw(e);
        }
        
        MatrixMode(PROJECTION_MATRIX);
        LoadIdentity();
        float r = GetClientHeight() / GetClientWidth();
        Frustum(-1, 1, -r, r, 1, 10);

        MatrixMode(TEXTURE_MATRIX);
        LoadIdentity();

        MatrixMode(MODELVIEW_MATRIX);
        LoadIdentity();

        Translate(0, 0, -3);
        static float dt = 0;
        dt += GetDeltaTime();
        Rotate(dt * 180, 0, 1, 0);

        glEnable(GL_DEPTH_TEST);
        //teapot->Draw();

        g.Begin();
        /*auto tex = alpha_buffer->GetTexture();
        g.DrawImage(*tex, Rect(0, 0, tex->GetWidth(), tex->GetHeight()), Rect(0, 0, GetClientWidth(), GetClientWidth()));*/
        
        //FontManager::CreateFont("", 30)->DrawString(g, Brush(Resources::LoadTexture("drawable/r").get()), "纹理字体", -1, 10, 50);
        
        string sss = "FPS: " + toStr(GetFPS()) + "   draw time:" + toStr((GetMicroSecond() - t) / 1000.0f) + "\n";
        FontManager::GetDefaultFont()->DrawString(g, Brushes::Black, sss, -1, Unit::ToPixel(1, Unit::UNIT_DP), Unit::ToPixel(1, Unit::UNIT_DP), GetClientWidth(), 0, FontFlag_Left);

        /*g.FillRect(Color::Blue, Rect(0, 0, GetClientWidth(), GetClientWidth()));
        Texture* tex = new Texture;
        *(int*)tex = FontManager::GetDefaultFont()->GetTextures()[0];
        *(((int*)tex) + 1) = 512;
        *(((int*)tex) + 2) = 512;
        g.DrawImage(*tex, Rect(0, 0, tex->GetWidth(), tex->GetHeight()), Rect(0, 0, tex->GetWidth(), tex->GetHeight()));*/

        //shared_ptr<Curve> c(new BezierCurve(0.5, 0, 0.5, 1));
        //c->Draw(g, Pens::Black, GetClientRect());

        glDisable(GL_STENCIL_TEST);
    }
};

shared_ptr<Drawable> MyWindow::Block::btn_gray;
shared_ptr<Drawable> MyWindow::Block::btn_red;

MyWindow w;

void SE_main(){
#	if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //_CrtSetBreakAlloc(1286);
#	endif

    //DEBUG_LOG("screen size: %d x %d\n", Window::GetScreenWidth(), Window::GetScreenHeight());

    w.SetLocation(Vector2(300, 10));
    w.SetSize(Vector2(Unit::ToPixel(7, Unit::UNIT_CM), Unit::ToPixel(11, Unit::UNIT_CM)));
    auto size = w.GetClientSize();
    cout << size.X() << "," << size.Y()<<endl;
    w.Run();
}
