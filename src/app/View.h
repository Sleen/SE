#pragma once

#include <stack>

#include "Macro.h"
#include "my_math.h"
#include "Font.h"
#include "Control.h"
#include "Texture.h"
#include "Bitmap.h"

#include "json11.hpp"
#include "Type.h"

//SE_BEGIN
namespace SEView{

USING_SE;
using namespace json11;

#define WRAP_CONTENT -1
#define MATCH_PARENT -2

#define UNSPECIFIED 1
#define EXACTLY     2
#define AT_MOST     3

#define LEFT    0
#define RIGHT   1
#define TOP     0
#define BOTTOM  2
#define CENTER_HORIZONTAL   4
#define CENTER_VERTICAL     8
#define CENTER              (CENTER_HORIZONTAL | CENTER_VERTICAL)

#define VERTICAL    0
#define HORIZONTAL  1

class Animation;

class Drawable : public Object{
    DECL_ABS_CLASS(Drawable)
protected:
    Padding padding;
    Vector2 size;
    float alpha = 1.0f;
    shared_ptr<Animation> ani;

public:
    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) = 0;

    virtual ~Drawable(){}

    void SetAlpha(float alpha){
        this->alpha = alpha;
    }

    void FadeIn(float duration = 0.3f, float delay = 0);

    void FadeOut(float duration = 0.3f, float delay = 0);

    void SetPadding(const Padding& padding){
        this->padding = padding;
    }

    virtual Padding GetPadding() const{
        return padding;
    }

    virtual Vector2 GetSize() const{
        return size + padding.GetSize();
    }
};

class ColorDrawable : public Drawable{
    DECL_CLASS(ColorDrawable);
protected:
    int color;

public:
    ColorDrawable(){
        color = Color::White;
    }

    ColorDrawable(int color){
        this->color = color;
    }

    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) override{
        g.FillRect(Brush(Color::AlphaColor(alpha, color)), rect);
    }
};

class TextureDrawable : public Drawable{
protected:
    shared_ptr<Texture> texture;
    SizeMode sizeMode;

public:
    TextureDrawable(shared_ptr<Texture> texture, SizeMode sizeMode = SizeMode::Strech){
        this->texture = texture;
        this->sizeMode = sizeMode;
        this->size = Vector2(texture->GetWidth(), texture->GetHeight());
    }

    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) override{
        g.DrawImage(*texture, rect, sizeMode, Color::AlphaColor(alpha, Color::White));
    }

    shared_ptr<Texture> GetTexture() const{
        return texture;
    }
};

class NinePatchTexture : public Texture{
protected:
    vector<int> verPoints;
    vector<int> horPoints;
    Padding padding;
    int fixedWidth = 0;
    int fixedHeight = 0;
    const int keyColor = Color::Black;

private:
    void loadBitmap(const Bitmap& bitmap){
        Bitmap bmp(bitmap.GetWidth() - 2, bitmap.GetHeight() - 2);
        int* data = (int*)bitmap.GetData();
        int* newData = (int*)bmp.GetData();
        int w = bmp.GetWidth();
        int h = bmp.GetHeight();
        for (int i = 0; i < w; i++){
            for (int j = 0; j < h; j++){
                newData[j*w + i] = data[(j + 1)*(w + 2) + i + 1];
            }
        }

        bool black = false;
        int p = 0;
        for (int i = 1; i <= w; i++){
            if ((data[i] == keyColor) != black){
                black = !black;
                horPoints.push_back(i - 1 - p);
                p = i - 1;
            }
        }
        horPoints.push_back(w-p);
       
        for (int i = 0, s = horPoints.size(); i < s; i+=2){
            fixedWidth += horPoints[i];
        }

        black = false;
        p = 0;
        int ww = w + 2;
        for (int i = 1; i <= h; i++){
            if ((data[i*ww] == keyColor) != black){
                black = !black;
                verPoints.push_back(i - 1 - p);
                p = i - 1;
            }
        }
        verPoints.push_back(h-p);

        for (int i = 0, s = verPoints.size(); i < s; i += 2){
            fixedHeight += verPoints[i];
        }

        int left = -1, top = -1, right = -1, bottom = -1;

        int* d = data + (w + 2)*(h + 1);
        for (int i = 1; i <= w; i++){
            if (d[i] == keyColor){
                left = i - 1;
                break;
            }
        }
        if (left < 0){
            left = right = 0;
        }
        else{
            for (int i = w; i >= 1; i--){
                if (d[i] == keyColor){
                    right = w - i;
                    break;
                }
            }
        }

        for (int i = 1; i <= h; i++){
            if (data[i*ww + w + 1] == keyColor){
                top = i - 1;
                break;
            }
        }
        if (top < 0){
            top = bottom = 0;
        }
        else{
            for (int i = h; i >= 1; i--){
                if (data[i*ww + w + 1] == keyColor){
                    bottom = h - i;
                    break;
                }
            }
        }
        padding.Set(left, top, right, bottom);

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.GetWidth(), bmp.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp.GetData());
        this->width = bmp.GetWidth();
        this->height = bmp.GetHeight();
    }

public:
    NinePatchTexture(const Bitmap& bitmap){
        loadBitmap(bitmap);
    }

    NinePatchTexture(const std::string& file){
        loadBitmap(Bitmap(file));
    }

    const vector<int>& GetVerPoints() const{
        return verPoints;
    }

    const vector<int>& GetHorPoints() const{
        return horPoints;
    }

    int GetFixedWidth() const{
        return fixedWidth;
    }

    int GetFixedHeight() const{
        return fixedHeight;
    }

    const Padding& GetPadding() const{
        return padding;
    }
};

class NinePatchTextureDrawable : public Drawable{
protected:
    shared_ptr<NinePatchTexture> texture;
    
public:
    NinePatchTextureDrawable(shared_ptr<NinePatchTexture> texture){
        this->texture = texture;
        this->padding = (Padding&)texture->GetPadding() * (Unit::ScreenDpi() / 160);
        this->size = Vector2(texture->GetWidth(), texture->GetHeight()) * (Unit::ScreenDpi() / 160);
    }

    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) override{
        float scale = Unit::ScreenDpi() / 160;
        auto hor = texture->GetHorPoints();
        auto ver = texture->GetVerPoints();

        int fw = texture->GetFixedWidth();
        int fh = texture->GetFixedHeight();

        int vw = texture->GetWidth() - fw;      // variable width
        int vh = texture->GetHeight() - fh;     // variable height

        // scalar
        //float sx = (float)(rect.GetWidth() - fw) / vw;
        //float sy = (float)(rect.GetHeight() - fh) / vh;
        float vsx = (rect.GetWidth() - fw * scale) / vw;
        float vsy = (rect.GetHeight() - fh * scale) / vh;
        float fsx = rect.GetWidth() / fw;
        float fsy = rect.GetHeight() / fh;
        vsx = max(0.0f, vsx);
        vsy = max(0.0f, vsy);
        fsx = min(1.0f, fsx) * scale;
        fsy = min(1.0f, fsy) * scale;

        int color = Color::AlphaColor(alpha, Color::White);

        float srcX = 0;
        float dstX = rect.Left();
        for (int i = 0, w = hor.size(); i < w; i++){
            float srcY = 0;
            float dstY = rect.Top();
            float rw = hor[i] * (i % 2 == 0 ? fsx : vsx);
            //float rw = i % 2 == 0 ? hor[i] : hor[i] * sx;   // real width
            for (int j = 0, h = ver.size(); j < h; j++){
                float rh = ver[j] * (j % 2 == 0 ? fsy : vsy);
                //float rh = j % 2 == 0 ? ver[j] : ver[j] * sy;       // real height
                g.DrawImage(*texture, Rect(srcX, srcY, hor[i], ver[j]), Rect(dstX, dstY, rw, rh), color);
                srcY += ver[j];
                dstY += rh;
            }
            srcX += hor[i];
            dstX += rw;
        }
    }
};

class ShapeDrawable : public Drawable{
    DECL_ABS_CLASS(ShapeDrawable);
protected:
    int strokeColor;
    int solidColor;
    float strokeWidth;

    ShapeDrawable(int solidColor, int strokeColor, float strokeWidth){
        this->solidColor = solidColor;
        this->strokeColor = strokeColor;
        this->strokeWidth = strokeWidth;
    }
};

class RectDrawable : public ShapeDrawable{
    DECL_CLASS(RectDrawable);
protected:
    float radius;
    int corner;

public:
    RectDrawable() 
        : ShapeDrawable(Color::Transparent, Color::Transparent, 1){
        corner = 15;
        radius = 0;
    }

    RectDrawable(int solidColor, int strokeColor = Color::Transparent, float strokeWidth = 1, float radius = 0, int corner = 15)
        : ShapeDrawable(solidColor, strokeColor, strokeWidth) {
        this->radius = radius;
        this->corner = corner;
    }

    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) override{
        if (solidColor != Color::Transparent){
            g.FillRoundRect(Brush(Color::AlphaColor(alpha, solidColor)), rect, radius, corner);
        }
        
        if (strokeColor != Color::Transparent && strokeWidth > 0){
            g.DrawRoundRect(Pen(Color::AlphaColor(alpha, strokeColor), strokeWidth), rect, radius, corner);
        }
    }
};

class OvalDrawable : public ShapeDrawable{
    DECL_CLASS(OvalDrawable);
public:
    OvalDrawable()
        : ShapeDrawable(Color::Transparent, Color::Transparent, 1) {

    }

    OvalDrawable(int solidColor, int strokeColor = Color::Transparent, float strokeWidth = 1)
        : ShapeDrawable(solidColor, strokeColor, strokeWidth) {
        
    }

    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) override{
        if (solidColor != Color::Transparent){
            g.FillOval(Brush(Color::AlphaColor(alpha, solidColor)), rect);
        }

        if (strokeColor != Color::Transparent && strokeWidth > 0){
            g.DrawOval(Pen(Color::AlphaColor(alpha, strokeColor), strokeWidth), rect);
        }
    }
};

class LineDrawable : public Drawable{
    DECL_CLASS(LineDrawable);
protected:
    int direction;
    int color;
    float width;

public:
    LineDrawable(){
        this->color = Color::Black;
        this->width = 1;
        this->direction = HORIZONTAL;
        size.Y() = 1;
    }

    LineDrawable(int color, float width = 1, int direction = HORIZONTAL){
        this->color = color;
        this->width = width;
        this->direction = direction;
        
        if (direction == HORIZONTAL){
            size.Y() = width;
        }
        else{
            size.X() = width;
        }
    }

    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) override{
        if (direction == HORIZONTAL){
            float center = (rect.Top() + rect.Bottom()) / 2;
            g.DrawLine(Pen(Color::AlphaColor(alpha, color), width), rect.Left(), center, rect.Right(), center);
        }
        else{
            float center = (rect.Left() + rect.Right()) / 2;
            g.DrawLine(Pen(Color::AlphaColor(alpha, color), width), center, rect.Top(), center, rect.Bottom());
        }
    }
};

class State{
private:
    unordered_map<string, void*> map;
    shared_ptr<Drawable> drawable;
    
public:
    shared_ptr<Drawable> GetDrawable() const{
        return drawable;
    }

    void SetDrawable(shared_ptr<Drawable> drawable){
        this->drawable = drawable;
    }

    void Put(string name, void* value){
        map[name] = value;
    }

    bool IsEmpty() const{
        return map.empty();
    }

    bool HasMouseEvent() const{
        for (auto& m : map){
            if (m.first == "mouseDown" || m.first == "mouseIn"){
                return true;
            }
        }
        return false;
    }

    bool Check(Object* obj) const{
        if (map.empty()){
            return true;
        }

        for (auto& m : map){
            auto field = obj->GetClass()->GetField(m.first);
            if (field){
                auto v = field->Get(obj);
                if (memcmp(v, m.second, field->GetType()->GetSize()) != 0)
                    return false;
            }
            else{
                return false;
            }
        }

        return true;
    }
};

class SelectDrawable : public Drawable{
    DECL_CLASS(SelectDrawable);
private:
    vector<State> states;
    Drawable* drawable = nullptr;

    void Update(Object* obj){
        for (auto& s : states){
            if (s.Check(obj)){
                drawable = s.GetDrawable().get();
                return;
            }
        }
        drawable = nullptr;
    }

public:
    virtual Padding GetPadding() const{
        if (drawable)
            return  drawable->GetPadding() + padding;
        return padding;
    }

    virtual Vector2 GetSize() const{
        if (drawable)
            return Drawable::GetSize() + GetPadding().GetSize();
        return size + padding.GetSize();
    }

    void AddState(const State& state){
        states.push_back(state);
        if (state.IsEmpty())
            drawable = state.GetDrawable().get();
    }

    bool HasMouseEvent() const{
        for (auto& s : states){
            if (s.HasMouseEvent()){
                return true;
            }
        }
        return false;
    }

    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) override{
        Update(obj);
        if (drawable){
            drawable->OnDraw(obj, g, rect);
        }
    }
};

class Sprite{
protected:
    shared_ptr<Texture> texture;
    Rect rect;

public:
    Sprite(shared_ptr<Texture> texture){
        this->texture = texture;
        this->rect = Rect(0, 0, texture->GetWidth(), texture->GetHeight());
    }

    Sprite(shared_ptr<Texture> texture, const Rect& rect){
        this->texture = texture;
        this->rect = rect;
    }

    Vector2 GetSize() const{
        return rect.GetSize();
    }
    
    //void Draw(Graphics& g, const Rect& src, const Rect& dest, int color = Color::White);

    void Draw(Graphics& g, const Rect& dest, int color = Color::White){
        g.DrawImage(*texture, rect, dest, color);
    }
};

class SpriteDrawable : public Drawable{
private:
    shared_ptr<Sprite> sprite;

public:
    SpriteDrawable(){
        
    }

    SpriteDrawable(shared_ptr<Sprite> sprite){
        this->sprite = sprite;
        this->size = sprite->GetSize();
    }

    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) override{
        sprite->Draw(g, rect);
    }
};

class FrameDrawable : public Drawable{
private:
    vector<shared_ptr<Sprite>> frames;
    int now = 0;

public:
    FrameDrawable(){

    }

    FrameDrawable(vector<shared_ptr<Sprite>> sprites){
        this->frames = sprites;
        for (auto& f : frames){
            auto s = f->GetSize();
            if (s.X() > size.X()) size.X() = s.X();
            if (s.Y() > size.Y()) size.Y() = s.Y();
        }
    }

    FrameDrawable(shared_ptr<Texture> texture, int columns) : FrameDrawable(texture, columns, 1, columns){
        
    }

    FrameDrawable(shared_ptr<Texture> texture, int frameCount, int rows, int columns){
        int frameWidth = texture->GetWidth() / columns;
        int frameHeight = texture->GetHeight() / rows;
        size = Vector2(frameWidth, frameHeight);

        for (int i = 0; i < rows; i++){
            for (int j = 0; j < columns; j++){
                int index = i*columns + j;
                if (index < frameCount){
                    frames.push_back(make_shared<Sprite>(texture, Rect(j*frameWidth, i*frameHeight, frameWidth, frameHeight)));
                }
            }
        }
    }

    virtual void OnDraw(Object* obj, Graphics& g, const Rect& rect) override{
        if (now >= 0 && now < frames.size()){
            frames[now]->Draw(g, rect);
            if (++now >= frames.size()) now = 0;
        }
    }
};

class Curve : public enable_shared_from_this<Curve>{
public:
    virtual float Value(float percent) = 0;
    virtual void Draw(const Graphics& g, const Pen& pen, const Rect& bounds, int segment = 100){
        float t = 1.0f / segment;
        g.Push();
        g.Translate(bounds.GetLocation() + Vector2(0, bounds.GetHeight()));
        g.Scale(bounds.GetWidth(), -bounds.GetHeight());
        for (float i = 0; i < 1; i += t)
        {
            float j = min(1.0f, i + t);
            g.DrawLine(pen, i, Value(i), j, Value(j));
        }
        g.Pop();
    }
    shared_ptr<Curve> In();
    shared_ptr<Curve> Out();
    shared_ptr<Curve> InOut();
    shared_ptr<Curve> Reverse();
    shared_ptr<Curve> operator+(shared_ptr<Curve> c);
    shared_ptr<Curve> Merge(shared_ptr<Curve> another, float position = 0.5f);
    shared_ptr<Curve> Repeat(float count = 2);
    virtual ~Curve(){}
};

class LinearCurve : public Curve{
public:
    virtual float Value(float percent){
        return percent;
    }
};

class BezierCurve : public Curve{
public:
    Vector2 p1, p2;

    BezierCurve(){
        p1(0, 0);
        p2(1, 1);
    }

    BezierCurve(float x1, float y1, float x2, float y2){
        p1(x1, y1);
        p2(x2, y2);
    }

    BezierCurve(const Vector2& c1, const Vector2& c2){
        p1(c1);
        p2(c2);
    }

    float GetY(float t) const{
        float t1 = 1 - t;
        return (t*t1*t1*p1.Y() + t*t*t1*p2.Y()) * 3 + t*t*t;
    }

    float GetX(float t) const{
        float t1 = 1 - t;
        return (t*t1*t1*p1.X() + t*t*t1*p2.X()) * 3 + t*t*t;
    }

    virtual float Value(float percent){
        int d = 20;		// 二分比较次数
        float min = 0;
        float max = 1;
        float mid;
        while (d-- > 0){
            mid = (min + max) / 2;
            float x = GetX(mid);
            if (x>percent)
                max = mid;
            else
                min = mid;
        }

        return GetY((max + min) / 2);
    }

};

class PowerCurve : public Curve
{
protected:
    float power;
public:
    void SetPower(float power){
        this->power = power;
    }
    float GetPower() const{
        return power;
    }
    PowerCurve(float p = 2) { power = p; }
    virtual float Value(float percent)
    {
        return 1 - (float)pow(1 - percent, power);
    }
};

class SineCurve : public Curve
{
    virtual float Value(float percent)
    {
        return (float)sin(percent * M_PI / 2);
    }
};

class CircleCurve : public Curve
{
    virtual float Value(float percent)
    {
        return (float)sqrt(1 - (percent -= 1) * percent);
    }
};

class ExponentCurve : public Curve
{
    virtual float Value(float percent)
    {
        return percent == 1 ? 1 : (-(float)pow(2, -10 * percent) + 1);
    }
};

class BounceCurve : public Curve
{
protected:
    float times, decay;
public:
    void SetTimes(float times){
        if (times < 1)times = 1;
        this->times = times;
    }
    void SetDecay(float decay){
        if (decay < 1.1f)decay = 1.1f;
        this->decay = decay;
    }
    float GetTimes() const{
        return times;
    }
    float GetDecay() const{
        return decay;
    }
    BounceCurve(float times = 4, float decay = 4)
    {
        this->times = times;
        this->decay = decay;
    }
    virtual float Value(float percent)
    {
        float d = 1 / (float)sqrt(decay);
        float x = 1 / (1 + d * 2 * (1 - (float)pow(d, times - 1)) / (1 - d));
        float n = x;
        float s = 1;
        while (n < percent) { n += x * 2 * d; x *= d; s /= decay; }
        return s / x / x * (percent -= n - x) * percent + 1 - s;
    }
};

class FlexCurve : public Curve
{
protected:
    float times;
public:
    void SetTimes(float times){
        if (times < 0)times = 0;
        this->times = times;
    }
    float GetTimes() const{
        return times;
    }
    FlexCurve(float times = 4)
    {
        this->times = times;
    }
    /*
    virtual ~FlexCurve(){
    cout << "FlexCurve->release\n";
    }*/
    virtual float Value(float percent)
    {
        return (float)(pow(2, -10 * percent) * (times == 0 ? -1 : sin((percent - 1 / times / 4) * (2 * M_PI) * times)) + 1);
    }
};

class BackCurve : public Curve
{
protected:
    float s;
public:
    float GetS() const{ return s; }
    void SetS(float s){ this->s = s; }
    BackCurve(float s = 1.70158f) { this->s = s; }
    virtual float Value(float percent)
    {
        return (percent -= 1) * percent * ((s + 1) * percent + s) + 1;
    }
};

class BlinkCurve : public Curve
{
protected:
    float position;

public:
    void SetPosition(float position){
        if (position < 0) position = 0;
        else if (position > 1) position = 1;
        this->position = position;
    }
    float GetPosition() const{
        return position;
    }

    BlinkCurve(float position = 0.5f)
    {
        this->position = position;
    }

    virtual float Value(float percent)
    {
        return percent >= position ? 1 : 0;
    }
};

class ModifyCurve : public Curve
{
protected:
    shared_ptr<Curve> curve;
    ModifyCurve(shared_ptr<Curve> curve) { this->curve = curve; }
public:
    void SetCurve(shared_ptr<Curve> curve){
        this->curve = curve;
    }
    shared_ptr<Curve> GetCurve(){
        return curve;
    }
};

class InCurve : public ModifyCurve
{
public:
    InCurve(shared_ptr<Curve> curve) : ModifyCurve(curve) { }

    virtual float Value(float percent)
    {
        return 1 - curve->Value(1 - percent);
    }
};

class MergeCurve : public ModifyCurve
{
protected:
    shared_ptr<Curve> curve2;
    float position;
public:
    void SetPosition(float position){
        if (position < 0) position = 0;
        else if (position > 1) position = 1;
        this->position = position;
    }
    float GetPosition() const{
        return position;
    }
    shared_ptr<Curve> GetCurve2(){
        return curve2;
    }
    void SetCurve2(shared_ptr<Curve> curve2){
        this->curve2 = curve2;
    }

    MergeCurve(shared_ptr<Curve> c1, shared_ptr<Curve> c2, float position = 0.5f)
        : ModifyCurve(c1)
    {
        this->curve2 = c2;
        this->position = position;
    }

    virtual float Value(float percent)
    {
        if (position == 0)
            return curve2->Value(percent);
        else if (position == 1)
            return curve->Value(percent);
        float p = 1 - position;
        return percent > position ? (curve2->Value((percent - position) / p) * p + position) : (curve->Value(percent / position) * position);
    }
};

class InOutCurve : public MergeCurve
{
public:
    InOutCurve(shared_ptr<Curve> curve, float position = 0.5f) : MergeCurve(shared_ptr<Curve>(new InCurve(curve)), curve, position) { }

};

class CombineCurve : public MergeCurve
{
public:
    CombineCurve(shared_ptr<Curve> c1, shared_ptr<Curve> c2, float position = 0.5f)
        : MergeCurve(c1, c2, position) { }
    virtual float Value(float percent)
    {
        if (position == 0)
            return curve2->Value(percent);
        else if (position == 1)
            return curve->Value(percent);
        float p = 1 - position;
        return percent > position ? curve2->Value((percent - position) / p) : curve->Value(percent / position);
    }
};

class RepeatCurve : public ModifyCurve
{
protected:
    float repeatCount;

public:
    void SetRepeatCount(float count){
        repeatCount = count;
    }
    float GetRepeatCount()const{
        return repeatCount;
    }
    //~RepeatCurve(){ cout << "RepeatCurve::~\n"; ModifyCurve::~ModifyCurve(); }
    RepeatCurve(shared_ptr<Curve> curve, float count) : ModifyCurve(curve) { this->repeatCount = count; }

    virtual float Value(float percent)
    {
        return curve->Value((percent *= repeatCount) - (int)percent);
    }
};

class ReverseCurve : public ModifyCurve
{
public:
    ReverseCurve(shared_ptr<Curve>) : ModifyCurve(curve) { }

    virtual float Value(float percent)
    {
        return curve->Value(1 - percent);
    }
};

class Animation;

class AnimationManager{
private:
    static list<shared_ptr<Animation>> animations;

public:
    static void Add(shared_ptr<Animation> animation){
        animations.push_back(animation);
    }

    static void Remove(shared_ptr<Animation> animation){
        animations.remove(animation);
    }

    static void Update(float dt);
};

class Animation : public enable_shared_from_this<Animation>{
protected:
    float duration = 0;
    float now = 0;
    bool finished = false;

public:
    Event<Animation> Start;
    Event<Animation> Step;
    Event<Animation> End;
    shared_ptr<Curve> curve;

    Animation(){

    }

    virtual ~Animation(){}

    Animation(float duration){
        this->duration = duration;
    }

    float GetDuration() const{
        return duration;
    }

    float GetPercent() const{
        float p = now / duration;
        return curve ? curve->Value(p) : p;
    }

    shared_ptr<Curve> GetCurve() const{
        return curve;
    }

    virtual void SetCurve(shared_ptr<Curve> curve){
        this->curve = curve;
    }

    virtual void OnStep(float percent) = 0;

    virtual void Reset(){
        now = 0;
        finished = false;
    }

    /*
        @return     <0,     not finished
                    >=0,    finished, the number means time of rest
    */
    virtual float Run(float dt){
        if (now == 0){
            Start(*this);
        }

        now += dt;
        if (now >= duration){
            dt = now - duration;
            now = duration;
        }
        else{
            dt = -1;
        }

        float percent = now / duration;
        OnStep(curve? curve->Value(percent) : percent);
        Step(*this);

        if (now == duration){
            finished = true;
            End(*this);
        }

        return dt;
    }

    bool isFinished(){
        return finished;
    }

    void Play(){
        AnimationManager::Add(this->shared_from_this());
    }
};

class AnimationSet : public Animation{
protected:
    list<shared_ptr<Animation>> anims;

public:
    virtual void Reset() override{
        for (auto& a : anims){
            a->Reset();
        }
        Animation::Reset();
    }

    virtual void Add(shared_ptr<Animation> anim){
        anims.push_back(anim);
        //anim->SetCurve(curve);
        OnAdd(anim);
    }

    virtual void OnAdd(shared_ptr<Animation> anim) = 0;
};

class Union : public AnimationSet{
public:
    virtual void OnAdd(shared_ptr<Animation> anim) override{
        float d = anim->GetDuration();
        if (duration >= 0 && (d < 0 || duration < d)){
            duration = d;
        }
    }

    virtual void SetCurve(shared_ptr<Curve> curve) override{
        for (auto& a : anims){
            a->SetCurve(curve);
        }
    }

    virtual void OnStep(float percent) override{

    }

    virtual float Run(float dt) override{
        if (now == 0){
            Start(*this);
        }

        now += dt;

        for (auto& a : anims){
            a->Run(dt);
        }

        if (now > duration){
            dt = now - duration;
            now = duration;
        }
        else {
            dt = -1;
        }

        float percent = now / duration;
        OnStep(curve ? curve->Value(percent) : percent);
        Step(*this);

        if (now == duration){
            finished = true;
            End(*this);
        }

        return dt;
    }
};

class Sequence : public AnimationSet{
protected:
    list<shared_ptr<Animation>>::iterator current;

public:
    virtual void OnAdd(shared_ptr<Animation> anim) override{
        float d = anim->GetDuration();
        if (duration >= 0 && d >= 0)
            duration += d;
        else
            duration = -1;
    }

    virtual void OnStep(float percent) override{
        
    }

    virtual float Run(float dt) override{
        if (now == 0){
            Start(*this);
            current = anims.begin();
        }
        if (dt == 0) return -1;
        now += dt;

        while (dt > 0 && current != anims.end()){
            dt = (*current)->Run(dt);

            if (dt >= 0){
                current++;
            }
        }

        if (now > duration){
            now = duration;
        }

        float percent = GetPercent();
        OnStep(curve ? curve->Value(percent) : percent);
        Step(*this);

        if (now == duration){
            finished = true;
            End(*this);
        }

        return dt;
    }
};

class Delay : public Animation{
public:
    Delay(float duration) : Animation(duration){

    }

    virtual void OnStep(float percent) override{

    }
};

template<typename T>
T GetValue(T a, T b, float percent){
    return a + b*percent;
}

template<typename T>
class ByAnimation : public Animation{
protected:
    Object* obj;
    Field* field;
    T start, by;

public:
    ByAnimation(Object* obj, Field* field, T by, float duration = 1.0f) : Animation(duration){
        this->obj = obj;
        this->field = field;
        this->by = by;
        this->Start += [this](Animation& ani){
            this->start = *(T*)this->field->Get(this->obj);
            // TODO    this line cannot be compiled on gcc
            //this->start = this->field->GetT<T>(this->obj);
        };
    }

    virtual void OnStep(float percent) override{
        field->Set<T>(obj, GetValue<T>(start, by, percent));
    }
};

template<typename T>
class ToAnimation : public Animation{
protected:
    Object* obj;
    Field* field;
    T start, to;

public:
    ToAnimation(Object* obj, Field* field, T to, float duration = 1.0f) : Animation(duration){
        this->obj = obj;
        this->field = field;
        this->to = to;
        this->Start += [this](Animation& ani){
            this->start = *(T*)this->field->Get(this->obj);
            // TODO    this line cannot be compiled on gcc
            //this->start = this->field->GetT<T>(this->obj);
        };
    }

    virtual void OnStep(float percent) override{
        field->Set<T>(obj, GetValue<T>(start, to - start, percent));
    }
};

class ViewGroup;

extern shared_ptr<RenderableTexture> alpha_buffer;

class View : public Object{
    DECL_CLASS(View);
protected:
    Padding padding, totalPadding;
    Padding margin;
    float measuredWidth, measuredHeight;
    float left = 0, top = 0;
    shared_ptr<Drawable> background;
    string id;
    Vector2 center = Vector2(0.5f, 0.5f);
    Vector2 scale = Vector2(1, 1);
    Vector2 translate = Vector2(0, 0);
    float rotate = 0.0f;
    float alpha = 1.0f;

    bool enabled = true, visibled = true;
    bool mouseIn = false, mouseDown = false;

    ViewGroup* parent = NULL;

    friend class ViewGroup;

    virtual void ResetTotalPadding(){
        totalPadding = background ? padding + background->GetPadding() : padding;
    }

    void DefaultMeasure(float preferredWidth, float preferredHeight, float width, float height, int widthMode, int heightMode){
        switch (widthMode){
        case EXACTLY:
            measuredWidth = width;
            break;
        case AT_MOST:
            measuredWidth = MIN(width, preferredWidth + totalPadding.GetWidth());
            break;
        default:
            measuredWidth = preferredWidth + totalPadding.GetWidth();
        }

        switch (heightMode){
        case EXACTLY:
            measuredHeight = height;
            break;
        case AT_MOST:
            measuredHeight = MIN(height, preferredHeight + totalPadding.GetHeight());
            break;
        default:
            measuredHeight = preferredHeight + totalPadding.GetHeight();
        }
    }

public:
    Event<View> Click, MouseEnter, MouseLeave;
    Event<View, MouseEventArgs> MouseDown, MouseUp, MouseMove;
    Event<View, KeyEventArgs> KeyDown, KeyUp, KeyPress;
    Event<View, Vector2> SizeChanged, LocationChanged;
    Event<View, DrawEventArgs> Draw;
    Event<View, float> Update;

    virtual ~View(){}

    // TODO   use a member transform matrix to improve performance
    Vector2 TransformPoint(const Vector2& point){
        Vector4 p(point.X(), point.Y(), 0, 1);
        p = TranslateMatrix(-translate.X(), -translate.Y(), 0) * p;
        p = TranslateMatrix(-center.X()*measuredWidth, -center.Y()*measuredHeight, 0) * p;
        p = RotateMatrix(-rotate, 0, 0, 1) * p;
        p = ScaleMatrix(1 / scale.X(), 1 / scale.Y(), 0) * p;
        p = TranslateMatrix(center.X()*measuredWidth, center.Y()*measuredHeight, 0) * p;
        return { p.X(), p.Y() };
    }

    bool ContainsPoint(const Vector2& point){
        auto p = TransformPoint(point);
        return GetBounds().Contains(p);
    }

    const string& GetId() const{
        return id;
    }

    void SetEnabled(bool enabled){
        this->enabled = enabled;
    }

    void SetVisibled(bool visibled){
        this->visibled = visibled;
    }

    Rect GetBounds(){
        return Rect(left, top, measuredWidth, measuredHeight);
    }

    ViewGroup* GetParent(){
        return parent;
    }

    bool IsEnabled() const{
        return enabled;
    }

    bool IsVisibled() const{
        return visibled;
    }

    virtual void OnKeyDown(KeyEventArgs e){
        KeyDown(*this, e);
    }

    virtual void OnKeyUp(KeyEventArgs e){
        KeyUp(*this, e);
    }

    virtual void OnKeyPress(KeyEventArgs e){
        KeyPress(*this, e);
    }

    virtual void OnMouseDown(MouseEventArgs e)
    {
        mouseDown = true;
        MouseDown(*this, e);
    }

    virtual void OnMouseMove(MouseEventArgs e){
        MouseMove(*this, e);
        if (mouseIn)
        {
            //if (!ClientRect.Contains(e.Location))
            //    OnMouseLeave(new EventArgs());
        }
        else
        {
            if (GetClientRect().Contains(e.X, e.Y))
                OnMouseEnter();
        }
    }

    virtual void OnMouseUp(MouseEventArgs e){
        mouseDown = false;
        MouseUp(*this, e);
        //if (mouseIn){
            if (!GetClientRect().Contains(e.X, e.Y))
                OnMouseLeave();
            else
                OnClick();
        //}
    }

    virtual void OnMouseEnter(){
        if (!mouseIn){
            mouseIn = true;
            MouseEnter(*this);
        }
    }

    virtual void OnMouseLeave(){
        if (mouseIn){
            mouseIn = false;
            MouseLeave(*this);
        }
    }

    virtual void OnClick(){
        Click(*this);
    }

    Rect GetClientRect(){
        return Rect(0, 0, measuredWidth, measuredHeight);
    }

    virtual void Measure(float width, float height, int widthMode, int heightMode){
        ResetTotalPadding();
        OnMeasure(width, height, widthMode, heightMode);
    }

    virtual void OnMeasure(float width, float height, int widthMode, int heightMode){
        DefaultMeasure(width, height, width, height, widthMode, heightMode);
    }

    virtual void OnLayout() { }

    void SetBackground(shared_ptr<Drawable> background){
        this->background = background;
        ResetTotalPadding();
    }

    virtual bool HasMouseEvent(){
        if (background && background->GetClass()->IsDerivedFrom(&SelectDrawable::clazz) && static_pointer_cast<SelectDrawable>(background)->HasMouseEvent())
            return true;
        return !Click.empty() || !MouseDown.empty() || !MouseUp.empty() || !MouseEnter.empty() || !MouseLeave.empty() || !MouseMove.empty();
    }

    shared_ptr<Drawable> GetBackground(){
        return background;
    }

    void SetBackColor(int color){
        SetBackground(shared_ptr<ColorDrawable>(new ColorDrawable(color)));
    }

    float GetMeasuredWidth() const{
        return measuredWidth;
    }

    float GetMeasuredHeight() const{
        return measuredHeight;
    }

    const Padding& GetPadding() const{
        return padding;
    }

    const Padding& GetMargin() const{
        return margin;
    }

    float GetLeft() const{
        return left;
    }

    float GetTop() const{
        return top;
    }

    float GetRight() const{
        return left + measuredWidth;
    }
    
    float GetBottom() const{
        return top + measuredHeight;
    }

    void SetPadding(const Padding& padding){
        this->padding = padding;
        ResetTotalPadding();
    }

    void SetMargin(const Padding& margin){
        this->margin = margin;
    }

    int GetDepth();
    bool bufferring = false;

    virtual void OnDraw(DrawEventArgs e){
        if (!visibled || measuredHeight <= 0 || measuredWidth <= 0)
            return;

        Graphics& g = e.GetGraphics();

        if (alpha < 1){
            alpha_buffer->Begin();
            bufferring = true;
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }

        Vector2 c(center.X()*GetMeasuredWidth(), center.Y()*GetMeasuredHeight());
        g.Push();
        g.Translate(translate);
        g.RotateAt(rotate, c);
        g.ScaleAt(scale, c);

        if (bufferring){
            glStencilFunc(GL_EQUAL, GetDepth() - 1, 0xffffffff);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        }

        OnDrawBack(e);

        if (measuredHeight > totalPadding.GetHeight() && measuredWidth > totalPadding.GetWidth()){
            g.Translate(totalPadding.GetLeft(), totalPadding.GetTop());

            glStencilFunc(GL_NEVER, 0, 0);
            glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);
            g.FillRect(Brushes::Black, 0, 0, measuredWidth - totalPadding.GetWidth(), measuredHeight - totalPadding.GetHeight());

            glStencilFunc(GL_EQUAL, GetDepth(), 0xffffffff);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

            OnDrawFore(e);

            glStencilFunc(GL_NEVER, 0, 0);
            glStencilOp(GL_DECR, GL_KEEP, GL_KEEP);
            g.FillRect(Brushes::Black, 0, 0, measuredWidth - totalPadding.GetWidth(), measuredHeight - totalPadding.GetHeight());

            if (bufferring){
                bufferring = false;
            }

            glStencilFunc(GL_EQUAL, GetDepth() - 1, 0xffffffff);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        }

        g.Translate(-totalPadding.GetLeft(), -totalPadding.GetTop());

        OnDrawOverlay(e);

        g.Pop();

        if (alpha < 1){
            alpha_buffer->End();
            bufferring = false;
            g.DrawImage(*alpha_buffer->GetTexture(), GetClientRect(), GetClientRect(), Color::AlphaColor(alpha, Color::White));
        }

    }

    virtual void OnDrawBack(DrawEventArgs e){
        if (background){
            background->OnDraw(this, e.GetGraphics(), Rect(0, 0, measuredWidth, measuredHeight));
        }
        //e.GetGraphics().DrawRect(Pens::Green, 0, 0, measuredWidth, measuredHeight);
    }

    virtual void OnDrawFore(DrawEventArgs e){

    }

    virtual void OnDrawOverlay(DrawEventArgs e){

    }

    virtual void Layout(float left, float top){
        this->left = left;
        this->top = top;

        OnLayout();
    }
};

class Label : public View{
    DECL_CLASS(Label);
protected:
    std::string text;
    //shared_ptr<Font> font;
    Font* font;
    int flags = FontFlag::FontFlag_Center | FontFlag::FontFlag_Middle;
    int color = Color::Gray;

public:
    Label(){
        font = Control::GetDefaultFont();
    }

    void SetFontFlag(int flags){
        this->flags = flags;
    }

    virtual void OnMeasure(float width, float height, int widthMode, int heightMode) override{
        if (widthMode == EXACTLY && heightMode == EXACTLY){
            measuredWidth = width;
            measuredHeight = height;
        }
        else{
            if (widthMode == UNSPECIFIED){
                width = -1;
            }

            Vector2 size = font->MeasureString(text, width>=0 ? (max(0.0f, width - totalPadding.GetWidth())) : width, height - totalPadding.GetHeight(), flags);

            switch (widthMode){
            case EXACTLY:
                measuredWidth = width;
                break;
            case AT_MOST:
                measuredWidth = MIN(width, size.X() + totalPadding.GetWidth());
                break;
            default:
                measuredWidth = size.X() + totalPadding.GetWidth();
            }

            switch (heightMode){
            case EXACTLY:
                measuredHeight = height;
                break;
            case AT_MOST:
                measuredHeight = MIN(height, size.Y() + totalPadding.GetHeight());
                break;
            default:
                measuredHeight = size.Y() + totalPadding.GetHeight();
            }
        }
    }

    void SetFont(Font* font){
        this->font = font;
    }

    Font* GetFont() const{
        return font;
    }

    void SetText(const std::string& text){
        this->text = text;
    }

    const std::string& GetText() const{
        return text;
    }

    void SetColor(int color){
        this->color = color;
    }

    int GetColor(){
        return color;
    }

    virtual void OnDrawFore(DrawEventArgs e) override{
        if (measuredWidth - totalPadding.GetWidth() <= 0){
            return;
        }

        Graphics& g = e.GetGraphics();
        font->DrawString(g, Brush(color), text, -1, 0, 0, measuredWidth - totalPadding.GetWidth(), measuredHeight - totalPadding.GetHeight(), flags);
    }
};

class ImageView : public View{
    DECL_CLASS(ImageView);
protected:
    shared_ptr<Drawable> drawable;
    
public:
    void SetTexture(shared_ptr<Texture> texture, SizeMode sizeMode = SizeMode::Zoom){
        SetDrawable(shared_ptr<Drawable>(new TextureDrawable(texture, sizeMode)));
    }

    void SetDrawable(shared_ptr<Drawable> drawable){
        this->drawable = drawable;
    }

    shared_ptr<Drawable> GetDrawable(){
        return drawable;
    }

    virtual bool HasMouseEvent() override{
        if (View::HasMouseEvent()){
            return true;
        }

        return drawable && drawable->GetClass()->IsDerivedFrom(&SelectDrawable::clazz) && static_pointer_cast<SelectDrawable>(drawable)->HasMouseEvent();
    }

    virtual void OnDrawFore(DrawEventArgs e) override{
        if (drawable){
            drawable->OnDraw(this, e.GetGraphics(), Rect(0, 0, measuredWidth - totalPadding.GetWidth(), measuredHeight - totalPadding.GetHeight()));
        }
    }

    virtual void OnMeasure(float width, float height, int widthMode, int heightMode) override{
        Vector2 size = drawable ? drawable->GetSize() : Vector2(0, 0);
        DefaultMeasure(size.X(), size.Y(), width, height, widthMode, heightMode);
    }
};

class Slider : public View{
protected:
    int orientation = HORIZONTAL;
    int minValue = 0;
    int maxValue = 100;
    int value = 0;
    float pageLength = 10;
    shared_ptr<Drawable> sliderDrawable;
    bool drag = false;
    Vector2 down, pos;

public:
    Slider(){
        background = shared_ptr<RectDrawable>(new RectDrawable(0x4f000000, 0, 0, 3));
        sliderDrawable = shared_ptr<RectDrawable>(new RectDrawable(0x6f000000, 0, 0, 3));
    }

    float GetPercent() const{
        return (float)(value - minValue) / (maxValue - minValue);
    }

    float GetPageCount(){
        return max(1.0f, (maxValue - minValue) / pageLength);
    }

    Rect GetSliderRect(){
        float percent = GetPercent();
        float w = measuredWidth - totalPadding.GetWidth();
        float h = measuredHeight - totalPadding.GetHeight();

        if (orientation == HORIZONTAL){
            float len = w / GetPageCount();
            return Rect(percent*(w - len), 0, len, h);
        }
        else{
            float len = h / GetPageCount();
            return Rect(0, percent*(h - len), w, len);
        }
    }

    virtual void OnMeasure(float width, float height, int widthMode, int heightMode) override{
        DefaultMeasure(6, 6, width, height, widthMode, heightMode);
    }

    virtual void OnDrawFore(DrawEventArgs e) override{
        if (sliderDrawable == nullptr)
            return;

        sliderDrawable->OnDraw(this, e.GetGraphics(), GetSliderRect());
    }

    virtual void OnMouseDown(MouseEventArgs e) override{
        if (e.Button == MouseButton::Left){
            Rect rect = GetSliderRect();
            down = e.Location();
            pos = down - rect.GetLocation();
            drag = rect.Contains(down);
        }
    }

    virtual void OnMouseUp(MouseEventArgs e) override{
        drag = false;
    }

    virtual void OnMouseMove(MouseEventArgs e) override{
        if (drag){
            Vector2 p = e.Location() - pos;
            if (orientation == HORIZONTAL){
                value = p.X() / ((measuredWidth - totalPadding.GetWidth())*(1 - 1 / GetPageCount())) * (maxValue - minValue) + minValue;
            }
            else{
                value = p.Y() / ((measuredHeight - totalPadding.GetHeight())*(1 - 1 / GetPageCount())) * (maxValue - minValue) + minValue;
            }
            if (value < minValue) value = minValue;
            if (value > maxValue) value = maxValue;
        }
    }
};

class ViewGroup : public View{
    DECL_CLASS(ViewGroup);
public:

    class LayoutParams : public Object{
        DECL_CLASS(ViewGroup::LayoutParams);
    protected:
        float width, height;

    public:
        LayoutParams(){
            width = WRAP_CONTENT;
            height = WRAP_CONTENT;
        }

        LayoutParams(float width, float height){
            this->width = width;
            this->height = height;
        }

        float GetWidth() const{
            return width;
        }

        float GetHeight() const{
            return height;
        }
    };

protected:
    vector<shared_ptr<View>> views;
    vector<shared_ptr<LayoutParams>> params;

    bool mousePreview = false;
    View *downControl = NULL, *moveControl = NULL;

    int GetChildMeasureMode(int measureMode, float childDimension){
        return  childDimension >= 0 ? EXACTLY : measureMode == UNSPECIFIED
            ? UNSPECIFIED : childDimension == MATCH_PARENT ? measureMode : AT_MOST;
    }

    float GetChildMeasureSize(float measureSize, float childDimension, float padding){
        return childDimension >= 0 ? MAX(0, childDimension) : measureSize - padding;
    }

public:

    virtual bool HasMouseEvent(){
        if (View::HasMouseEvent())
            return true;
        for (auto& v : views){
            if (v->HasMouseEvent())
                return true;
        }
        return false;
    }

    virtual void OnMouseDown(MouseEventArgs e)
    {
        e.X -= totalPadding.GetLeft();
        e.Y -= totalPadding.GetTop();

        if (mousePreview)
        {
            downControl = this;
            View::OnMouseDown(e);
        }
        else
        {
            for (auto c = views.rbegin(); c != views.rend(); c++)
            {
                if ((*c)->IsVisibled() && (*c)->IsEnabled() && (*c)->HasMouseEvent() && (*c)->ContainsPoint(e.Location()))//(*c)->GetBounds().Contains(e.Location()))
                {
                    auto p = (*c)->TransformPoint(e.Location());
                    e.X = p.X();
                    e.Y = p.Y();
                    downControl = (*c).get();
                    (*c)->OnMouseDown(MouseEventArgs(e.X - (*c)->GetBounds().Left(), e.Y - (*c)->GetBounds().Top(), e.Button, e.Action, e.Delta));
                    return;
                }
            }
            downControl = this;
            View::OnMouseDown(e);
        }
    }

    virtual void OnMouseMove(MouseEventArgs e)
    {
        auto p = TransformPoint(e.Location());
        e.X = p.X();
        e.Y = p.Y();

        e.X -= totalPadding.GetLeft();
        e.Y -= totalPadding.GetTop();
        
        View *con = NULL;
        if (downControl)
        {
            if (downControl == this)
            {
                con = this;
            }
            else
            {
                con = downControl;
            }
        }
        else
        {
            if (mousePreview)
            {
                con = this;
            }
            else
            {
                for (auto c = views.rbegin(); c != views.rend(); c++)
                {
                    if ((*c)->IsVisibled() && (*c)->IsEnabled() && (*c)->ContainsPoint(e.Location()))//(*c)->GetBounds().Contains(e.Location()))
                    {
                        con = (*c).get();
                        break;
                    }
                }
                if (!con)
                {
                    con = this;
                }
            }
        }
        if (moveControl && con != moveControl)
        {
            moveControl->OnMouseLeave();
            //com.OnMouseEnter(new EventArgs());
        }
        moveControl = con;
        if (con == this)
        {
            View::OnMouseMove(e);
        }
        else if (con)
        {
            auto p = con->TransformPoint(e.Location());
            e.X = p.X();
            e.Y = p.Y();
            con->OnMouseMove(MouseEventArgs(e.X - con->GetBounds().Left(), e.Y - con->GetBounds().Top(), e.Button, e.Action, e.Delta));
        }
    }

    virtual void OnMouseUp(MouseEventArgs e)
    {
        if (downControl != 0)
        {
            if (downControl == this)
            {
                View::OnMouseUp(e);
            }
            else
            {
                e.X -= totalPadding.GetLeft();
                e.Y -= totalPadding.GetTop();

                auto p = downControl->TransformPoint(e.Location());
                e.X = p.X();
                e.Y = p.Y();
                downControl->OnMouseUp(MouseEventArgs(e.X - downControl->GetBounds().Left(), e.Y - downControl->GetBounds().Top(), e.Button, e.Action, e.Delta));
            }
            downControl = 0;
        }
        else
        {
            cout << "something goes wrong\n";
        }
    }

    virtual void OnKeyDown(KeyEventArgs e)
    {
        View::OnKeyDown(e);
        for (auto c = views.begin(); c != views.end(); c++)
        {
            (*c)->OnKeyDown(e);
        }
    }

    virtual void OnKeyUp(KeyEventArgs e)
    {
        View::OnKeyUp(e);
        for (auto c = views.begin(); c != views.end(); c++)
        {
            (*c)->OnKeyUp(e);
        }
    }

    virtual void OnKeyPress(KeyEventArgs e)
    {
        View::OnKeyPress(e);
        for (auto c = views.begin(); c != views.end(); c++)
        {
            (*c)->OnKeyPress(e);
        }
    }

    void Add(shared_ptr<View> view, shared_ptr<LayoutParams> param){
        view->parent = this;
        views.push_back(view);
        params.push_back(param);
    }

    virtual void OnDrawFore(DrawEventArgs e){
        if (measuredHeight <= 0 || measuredWidth <= 0)
            return;

        Graphics& g = e.GetGraphics();

        for (int i = 0, len = views.size(); i < len; i++){
            auto view = views[i];

            g.Push();
            g.Translate(view->GetLeft(), view->GetTop());

            views[i]->OnDraw(e);

            g.Pop();
        }
        //Control::GetDefaultFont()->DrawString(g, Brushes::Green, mouseIn ? "true" : "false", -1, 0, 0);
    }
};

using LayoutParams = ViewGroup::LayoutParams;

#define SHOW_DIVIDER_BEGINNING      1
#define SHOW_DIVIDER_MIDDLE         2
#define SHOW_DIVIDER_END            4

class LinearLayout : public ViewGroup{
    DECL_CLASS(LinearLayout);
protected:
    int orientation = VERTICAL;
    
    int dividers = SHOW_DIVIDER_MIDDLE;
    shared_ptr<Drawable> dividerDrawable;

    bool hasDivider(int index){
        if (index == 0){
            return (dividers & SHOW_DIVIDER_BEGINNING) != 0;
        }
        else if (index == views.size()){
            return (dividers & SHOW_DIVIDER_END) != 0;
        }
        else{
            return (dividers & SHOW_DIVIDER_MIDDLE) != 0;
        }
    }

public:

    class LayoutParams : public ViewGroup::LayoutParams{
        DECL_CLASS(LinearLayout::LayoutParams);
    protected:
        float weight;
        int gravity;

    public:
        LayoutParams() : ViewGroup::LayoutParams(WRAP_CONTENT, WRAP_CONTENT){
            this->weight = 0;
            this->gravity = 0;
        }

        LayoutParams(float width, float height, float weight = 0, int gravity = 0)
            : ViewGroup::LayoutParams(width, height)
        {
            this->weight = weight;
            this->gravity = gravity;
        }

        float GetWeight() const{
            return weight;
        }

        float GetGravity() const{
            return gravity;
        }
    };

    void SetDividerDrawable(shared_ptr<Drawable> dividerDrawable){
        this->dividerDrawable = dividerDrawable;
    }

    void SetOrientation(int orientation){
        this->orientation = orientation;
    }

    /*
                        EXACTLY         AT_MOST         UNSPECIFIED
    
        >=0             EXACTLY         EXACTLY         EXACTLY
        MATCH_PARENT    EXACTLY         AT_MOST         UNSPECIFIED
        WRAP_CONTENT    AT_MOST         AT_MOST         UNSPECIFIED
    */

    virtual void OnDrawFore(DrawEventArgs e) override{
        if (dividerDrawable && dividers){
            Graphics& g = e.GetGraphics();

            float width, height;
            if (orientation == VERTICAL){
                width = measuredWidth - totalPadding.GetWidth();
                height = dividerDrawable->GetSize().Y();
            }
            else{
                width = dividerDrawable->GetSize().X();
                height = measuredHeight - totalPadding.GetHeight();
            }

            if (hasDivider(0)){
                dividerDrawable->OnDraw(this, g, Rect(0, 0, width, height));
            }
            for (int i = 0, n = views.size(); i < n; i++){
                if (hasDivider(i + 1)){
                    if (orientation == VERTICAL){
                        dividerDrawable->OnDraw(this, g, Rect(0, views[i]->GetBottom() + views[i]->GetMargin().GetBottom(), width, height));
                        //g.DrawRect(Pens::Blue, Rect(0, views[i]->GetBottom() + views[i]->GetMargin().GetBottom(), width, height));
                    }
                    else{
                        dividerDrawable->OnDraw(this, g, Rect(views[i]->GetRight() + views[i]->GetMargin().GetRight(), 0, width, height));
                    }
                }
            }
        }

        ViewGroup::OnDrawFore(e);
    }

    void MeasureVertical(float width, float height, int widthMode, int heightMode){
        float oldWidth = width;
        float oldHeight = height;

        width -= totalPadding.GetWidth();
        height -= totalPadding.GetHeight();

        bool allFillParent = true;
        float totalWeight = 0;
        int count = views.size();

        float maxFillWidth = 0;     // just works when all children set width to match_parent
        float maxWidth = 0;
        
        float totalLength = 0;

        for (int i = 0; i < count; i++){
            auto view = views[i];
            auto param = static_cast<LayoutParams*>(params[i].get());

            float w = param->GetWidth();
            float h = param->GetHeight();
            float weight = param->GetWeight();

            totalWeight += weight;

            allFillParent = allFillParent && param->GetWidth() == MATCH_PARENT;

            if (weight == 0){
                if (heightMode != UNSPECIFIED && height <= 0){
                    view->Measure(0, 0, EXACTLY, EXACTLY);
                    continue;
                }

                if (dividerDrawable && hasDivider(i)){
                    totalLength += dividerDrawable->GetSize().Y();
                    if (heightMode != UNSPECIFIED){
                        height -= dividerDrawable->GetSize().Y();
                    }
                }

                view->Measure(GetChildMeasureSize(width, w, view->GetMargin().GetWidth()),
                    GetChildMeasureSize(height, h, view->GetMargin().GetHeight()),
                    GetChildMeasureMode(widthMode, w), GetChildMeasureMode(heightMode, h));

                float mHeight = view->GetMeasuredHeight() + view->GetMargin().GetHeight();
                float mWidth = view->GetMeasuredWidth() + view->GetMargin().GetWidth();

                totalLength += mHeight;

                if (heightMode != UNSPECIFIED){
                    height -= mHeight;
                }

                if (mWidth > maxFillWidth){
                    maxFillWidth = mWidth;
                }

                if (w != MATCH_PARENT && mWidth > maxWidth){
                    maxWidth = mWidth;
                }
            }
        }

        for (int i = 0; i < count; i++){
            auto view = views[i];
            auto param = static_cast<LayoutParams*>(params[i].get());

            float w = param->GetWidth();
            float h = param->GetHeight();
            float weight = param->GetWeight();

            if (weight != 0){
                if (heightMode != UNSPECIFIED && height <= 0){
                    view->Measure(0, 0, EXACTLY, EXACTLY);
                    continue;
                }

                if (dividerDrawable && hasDivider(i)){
                    totalLength += dividerDrawable->GetSize().Y();
                    if (heightMode != UNSPECIFIED){
                        height -= dividerDrawable->GetSize().Y();
                    }
                }

                view->Measure(GetChildMeasureSize(width, w, view->GetMargin().GetWidth()),
                    GetChildMeasureSize(height*weight/totalWeight, h, view->GetMargin().GetHeight()),
                    GetChildMeasureMode(widthMode, w), EXACTLY);

                float mHeight = view->GetMeasuredHeight() + view->GetMargin().GetHeight();
                float mWidth = view->GetMeasuredWidth() + view->GetMargin().GetWidth();

                totalLength += mHeight;

                if (heightMode != UNSPECIFIED){
                    height -= mHeight;
                }

                if (mWidth > maxFillWidth){
                    maxFillWidth = mWidth;
                }

                if (w != MATCH_PARENT && mWidth > maxWidth){
                    maxWidth = mWidth;
                }

                totalWeight -= weight;
            }
        }

        if (allFillParent && widthMode != EXACTLY){
            maxWidth = maxFillWidth;
        }

        DefaultMeasure(maxWidth, totalLength, oldWidth, oldHeight, widthMode, heightMode);

        /*View::OnMeasure(widthMode == EXACTLY ? oldWidth : maxWidth + totalPadding.GetWidth(),
            heightMode == EXACTLY ? oldHeight : totalLength + totalPadding.GetHeight(), widthMode, heightMode);*/

        if (widthMode == EXACTLY){
            maxWidth = oldWidth - totalPadding.GetWidth();
        }

        height = oldHeight - totalPadding.GetHeight();
        for (int i = 0; i < count; i++){
            auto view = views[i];
            float w = static_cast<LayoutParams*>(params[i].get())->GetWidth();
            float weight = static_cast<LayoutParams*>(params[i].get())->GetWeight();
            totalWeight += weight;
            
            if (weight == 0){
                if (heightMode != UNSPECIFIED && height <= 0){
                    view->Measure(0, 0, EXACTLY, EXACTLY);
                    continue;
                }

                if (w == MATCH_PARENT && view->GetMeasuredWidth() != maxWidth - view->GetMargin().GetWidth()){
                    // TODO
                    view->Measure(maxWidth - view->GetMargin().GetWidth(),
                        GetChildMeasureSize(height, static_cast<LayoutParams*>(params[i].get())->GetHeight(), view->GetMargin().GetHeight()), EXACTLY, GetChildMeasureMode(heightMode, static_cast<LayoutParams*>(params[i].get())->GetHeight()));

                }
                    if (heightMode != UNSPECIFIED){
                        height -= view->GetMeasuredHeight() + view->GetMargin().GetHeight();
                    }
            }
        }
        for (int i = 0; i < count; i++){
            auto view = views[i];
            float w = static_cast<LayoutParams*>(params[i].get())->GetWidth();
            float weight = static_cast<LayoutParams*>(params[i].get())->GetWeight();

            if (weight != 0){
                if (heightMode != UNSPECIFIED && height <= 0){
                    view->Measure(0, 0, EXACTLY, EXACTLY);
                    continue;
                }

                if (w == MATCH_PARENT && view->GetMeasuredWidth() != maxWidth - view->GetMargin().GetWidth()){
                    // TODO
                    view->Measure(maxWidth - view->GetMargin().GetWidth(),
                        GetChildMeasureSize(height*weight / totalWeight, static_cast<LayoutParams*>(params[i].get())->GetHeight(), view->GetMargin().GetHeight()), EXACTLY, EXACTLY);//GetChildMeasureMode(heightMode, static_cast<LayoutParams*>(params[i].get())->GetHeight()));

                }
                    totalWeight -= weight;
                    if (heightMode != UNSPECIFIED){
                        height -= view->GetMeasuredHeight() + view->GetMargin().GetHeight();
                    }
            }
        }
    }

    void MeasureHorizontal(float width, float height, int widthMode, int heightMode){
        float oldWidth = width;
        float oldHeight = height;

        width -= totalPadding.GetWidth();
        height -= totalPadding.GetHeight();

        bool allFillParent = true;
        float totalWeight = 0;
        int count = views.size();

        float maxFillHeight = 0;     // just works when all children set height to match_parent
        float maxHeight = 0;

        float totalLength = 0;

        for (int i = 0; i < count; i++){
            auto view = views[i];
            auto param = static_cast<LayoutParams*>(params[i].get());

            float w = param->GetWidth();
            float h = param->GetHeight();
            float weight = param->GetWeight();

            totalWeight += weight;

            allFillParent = allFillParent && param->GetHeight() == MATCH_PARENT;

            if (weight == 0){
                if (widthMode != UNSPECIFIED && width <= 0){
                    view->Measure(0, 0, EXACTLY, EXACTLY);
                    continue;
                }

                if (dividerDrawable && hasDivider(i)){
                    totalLength += dividerDrawable->GetSize().X();
                    if (widthMode != UNSPECIFIED){
                        width -= dividerDrawable->GetSize().X();
                    }
                }

                view->Measure(GetChildMeasureSize(width, w, view->GetMargin().GetWidth()),
                    GetChildMeasureSize(height, h, view->GetMargin().GetHeight()),
                    GetChildMeasureMode(widthMode, w), GetChildMeasureMode(heightMode, h));

                float mHeight = view->GetMeasuredHeight() + view->GetMargin().GetHeight();
                float mWidth = view->GetMeasuredWidth() + view->GetMargin().GetWidth();

                totalLength += mWidth;

                if (widthMode != UNSPECIFIED){
                    width -= mWidth;
                }

                if (mHeight > maxFillHeight){
                    maxFillHeight = mHeight;
                }

                if (h != MATCH_PARENT && mHeight > maxHeight){
                    maxHeight = mHeight;
                }
            }
        }

        for (int i = 0; i < count; i++){
            auto view = views[i];
            auto param = static_cast<LayoutParams*>(params[i].get());

            float w = param->GetWidth();
            float h = param->GetHeight();
            float weight = param->GetWeight();

            if (weight != 0){
                if (widthMode != UNSPECIFIED && width <= 0){
                    view->Measure(0, 0, EXACTLY, EXACTLY);
                    continue;
                }

                if (dividerDrawable && hasDivider(i)){
                    totalLength += dividerDrawable->GetSize().X();
                    if (widthMode != UNSPECIFIED){
                        width -= dividerDrawable->GetSize().X();
                    }
                }

                view->Measure(GetChildMeasureSize(width*weight / totalWeight, w, view->GetMargin().GetWidth()),
                    GetChildMeasureSize(height, h, view->GetMargin().GetHeight()),
                    EXACTLY, GetChildMeasureMode(heightMode, h));

                float mHeight = view->GetMeasuredHeight() + view->GetMargin().GetHeight();
                float mWidth = view->GetMeasuredWidth() + view->GetMargin().GetWidth();

                totalLength += mWidth;

                if (widthMode != UNSPECIFIED){
                    width -= mWidth;
                }

                if (mHeight > maxFillHeight){
                    maxFillHeight = mHeight;
                }

                if (h != MATCH_PARENT && mHeight > maxHeight){
                    maxHeight = mHeight;
                }

                totalWeight -= weight;
            }
        }

        if (allFillParent && heightMode != EXACTLY){
            maxHeight = maxFillHeight;
        }
        
        DefaultMeasure(totalLength, maxHeight, oldWidth, oldHeight, widthMode, heightMode);

        /*View::OnMeasure(widthMode == EXACTLY ? oldWidth : totalLength + totalPadding.GetWidth(),
            heightMode == EXACTLY ? oldHeight : maxHeight + totalPadding.GetHeight(), widthMode, heightMode);*/

        if (heightMode == EXACTLY){
            maxHeight = oldHeight - totalPadding.GetHeight();
        }

        for (int i = 0; i < count; i++){
            auto view = views[i];
            float h = static_cast<LayoutParams*>(params[i].get())->GetHeight();

            if (h == MATCH_PARENT && view->GetMeasuredHeight() != maxHeight - view->GetMargin().GetHeight()){
                view->Measure(view->GetMeasuredWidth(), maxHeight - view->GetMargin().GetHeight(), EXACTLY, EXACTLY);
            }
        }
    }

    virtual void OnMeasure(float width, float height, int widthMode, int heightMode) override{
        if (orientation == VERTICAL){
            MeasureVertical(width, height, widthMode, heightMode);
        }
        else{
            MeasureHorizontal(width, height, widthMode, heightMode);
        }
    }

    virtual void OnLayout(){
        int len = views.size();
        if (len == 0) return;

        float width = measuredWidth - totalPadding.GetWidth();
        float height = measuredHeight - totalPadding.GetHeight();

        if (orientation == VERTICAL){
            for (int i = 0; i < len; i++){
                int gravity = static_cast<LayoutParams*>(params[i].get())->GetGravity();
                float w = width - views[i]->GetMeasuredWidth() - views[i]->GetMargin().GetWidth();
                float left = views[i]->GetMargin().GetLeft() + (((gravity & RIGHT) != 0) ? w : ((gravity & CENTER_HORIZONTAL) != 0) ? w / 2 : 0);
                float top = i == 0 ? views[0]->GetMargin().GetTop() : views[i - 1]->GetBottom() + views[i - 1]->GetMargin().GetBottom() + views[i]->GetMargin().GetTop() + (dividerDrawable && hasDivider(i) ? dividerDrawable->GetSize().Y() : 0);
                views[i]->Layout(left, top);
            }
        }
        else{
            for (int i = 0; i < len; i++){
                int gravity = static_cast<LayoutParams*>(params[i].get())->GetGravity();
                float h = height - views[i]->GetMeasuredHeight() - views[i]->GetMargin().GetHeight();
                float left = i == 0 ? views[0]->GetMargin().GetLeft() : views[i - 1]->GetRight() + views[i - 1]->GetMargin().GetRight() + views[i]->GetMargin().GetLeft() + (dividerDrawable && hasDivider(i) ? dividerDrawable->GetSize().X() : 0);
                float top = views[i]->GetMargin().GetTop() + (((gravity & BOTTOM) != 0) ? h : ((gravity & CENTER_VERTICAL) != 0) ? (h / 2) : 0);
                views[i]->Layout(left, top);
            }
        }
    }

};

class FrameLayout : public ViewGroup{
    DECL_CLASS(FrameLayout);
public:

    class LayoutParams : public ViewGroup::LayoutParams{
        DECL_CLASS(FrameLayout::LayoutParams);
    protected:
        int gravity;

    public:
        LayoutParams() : ViewGroup::LayoutParams(WRAP_CONTENT, WRAP_CONTENT){
            this->gravity = 0;
        }

        LayoutParams(float width, float height, int gravity = 0)
            : ViewGroup::LayoutParams(width, height)
        {
            this->gravity = gravity;
        }

        float GetGravity() const{
            return gravity;
        }
    };

    virtual void OnMeasure(float width, float height, int widthMode, int heightMode) override{
        float oldWidth = width;
        float oldHeight = height;

        width -= totalPadding.GetWidth();
        height -= totalPadding.GetHeight();

        bool allFillParentX = true;
        bool allFillParentY = true;
        int count = views.size();

        float maxFillWidth = 0;     // just works when all children set width to match_parent
        float maxWidth = 0;

        float maxFillHeight = 0;     // just works when all children set height to match_parent
        float maxHeight = 0;

        for (int i = 0; i < count; i++){
            auto view = views[i];
            auto param = static_cast<LayoutParams*>(params[i].get());

            float w = param->GetWidth();
            float h = param->GetHeight();

            allFillParentX = allFillParentX && param->GetWidth() == MATCH_PARENT;
            allFillParentY = allFillParentY && param->GetHeight() == MATCH_PARENT;

            if (heightMode != UNSPECIFIED && height <= 0){
                view->Measure(0, 0, EXACTLY, EXACTLY);
                continue;
            }

            view->Measure(GetChildMeasureSize(width, w, view->GetMargin().GetWidth()),
                GetChildMeasureSize(height, h, view->GetMargin().GetHeight()),
                GetChildMeasureMode(widthMode, w), GetChildMeasureMode(heightMode, h));

            float mHeight = view->GetMeasuredHeight() + view->GetMargin().GetHeight();
            float mWidth = view->GetMeasuredWidth() + view->GetMargin().GetWidth();

            if (mWidth > maxFillWidth){
                maxFillWidth = mWidth;
            }

            if (w != MATCH_PARENT && mWidth > maxWidth){
                maxWidth = mWidth;
            }

            if (mHeight > maxFillHeight){
                maxFillHeight = mHeight;
            }

            if (h != MATCH_PARENT && mHeight > maxHeight){
                maxHeight = mHeight;
            }
        }

        if (allFillParentX && widthMode != EXACTLY){
            maxWidth = maxFillWidth;
        }

        if (allFillParentY && heightMode != EXACTLY){
            maxHeight = maxFillHeight;
        }

        View::OnMeasure(widthMode == EXACTLY ? oldWidth : maxWidth + totalPadding.GetWidth(),
            heightMode == EXACTLY ? oldHeight : maxHeight + totalPadding.GetHeight(), widthMode, heightMode);

        float mw = measuredWidth - totalPadding.GetWidth();
        float mh = measuredHeight - totalPadding.GetHeight();

        for (int i = 0; i < count; i++){
            auto view = views[i];
            float w = static_cast<LayoutParams*>(params[i].get())->GetWidth();
            float h = static_cast<LayoutParams*>(params[i].get())->GetHeight();

            view->Measure(GetChildMeasureSize(mw, w, view->GetMargin().GetWidth()), GetChildMeasureSize(mh, h, view->GetMargin().GetHeight()),
                GetChildMeasureMode(EXACTLY, params[i]->GetWidth()), GetChildMeasureMode(EXACTLY, params[i]->GetHeight()));
        }
    }

    virtual void OnLayout(){
        float width = measuredWidth - totalPadding.GetWidth();
        float height = measuredHeight - totalPadding.GetHeight();

        for (int i = 0, len = views.size(); i < len; i++){
            int gravity = static_cast<LayoutParams*>(params[i].get())->GetGravity();
            float w = width - views[i]->GetMeasuredWidth() - views[i]->GetMargin().GetWidth();
            float h = height - views[i]->GetMeasuredHeight() - views[i]->GetMargin().GetHeight();
            float left = views[i]->GetMargin().GetLeft() + (((gravity & RIGHT) != 0) ? w : ((gravity & CENTER_HORIZONTAL) != 0) ? w / 2 : 0);
            float top = views[i]->GetMargin().GetTop() + (((gravity & BOTTOM) != 0) ? h : ((gravity & CENTER_VERTICAL) != 0) ? (h / 2) : 0);
            views[i]->Layout(left, top);
        }
    }
};

#define SCROLLBAR_INSIDE_OVERLAY    0
#define SCROLLBAR_INSIDE_INSETS     1
#define SCROLLBAR_OUTSIDE_OVERLAY   2
#define SCROLLBAR_OUTSIDE_INSETS    3

#define SCROLLBAR_NONE          0
#define SCROLLBAR_VERTICAL      1
#define SCROLLBAR_HORIZONTAL    2
#define SCROLLBAR_BOTH          (SCROLLBAR_VERTICAL | SCROLLBAR_HORIZONTAL)
#define SCROLLBAR_AUTO          4

class ScrollView : public FrameLayout{
    DECL_CLASS(ScrollView);
protected:
    shared_ptr<Drawable> verScrollTrack, verScrollThumb;
    shared_ptr<Drawable> horScrollTrack, horScrollThumb;
    shared_ptr<Animation> ani, ani2;

    float scrollX = 0;
    float scrollY = 0;
    float vx = 0;
    float vy = 0;
    float scrollBarMinSize = 6;
    int scrollBarStyle = SCROLLBAR_INSIDE_OVERLAY;
    int verScrollBarPosition = RIGHT;
    int horScrollBarPosition = BOTTOM;
    int scrollBarVisibility = SCROLLBAR_AUTO;
    int scrollability = SCROLLBAR_VERTICAL;

    bool dragging = false;
    Vector2 down, move1, move2;
    int64_t moveTime1, moveTime2;

    float getVerScrollBarSize(){
        float trackSize = verScrollTrack ? verScrollTrack->GetSize().X() : 0;
        float thumbSize = verScrollThumb ? verScrollThumb->GetSize().X() : 0;
        return max(scrollBarMinSize, max(trackSize, thumbSize));
    }

    float getHorScrollBarSize(){
        float trackSize = horScrollTrack ? horScrollTrack->GetSize().Y() : 0;
        float thumbSize = horScrollThumb ? horScrollThumb->GetSize().Y() : 0;
        return max(scrollBarMinSize, max(trackSize, thumbSize));
    }

    float getContentWidth(){
        return views.empty() ? 0 : (views[0]->GetMeasuredWidth() + views[0]->GetMargin().GetWidth());
    }

    float getContentHeight(){
        return views.empty() ? 0 : (views[0]->GetMeasuredHeight() + views[0]->GetMargin().GetHeight());
    }

    bool isVerScrollable(){
        if (scrollability == SCROLLBAR_AUTO) return getContentHeight() > measuredHeight - totalPadding.GetHeight();
        return scrollability & SCROLLBAR_VERTICAL;
    }

    bool isHorScrollable(){
        if (scrollability == SCROLLBAR_AUTO) return getContentWidth() > measuredWidth - totalPadding.GetWidth();
        return scrollability & SCROLLBAR_HORIZONTAL;
    }

    bool isScrollable(){
        return isVerScrollable() || isHorScrollable();
    }

    bool isVerScrollBarVisibled(){
        if (scrollBarVisibility == SCROLLBAR_AUTO) return isVerScrollable() && getContentHeight() > measuredHeight - totalPadding.GetHeight();
        return (scrollBarVisibility & SCROLLBAR_VERTICAL) && getContentHeight() > measuredHeight - totalPadding.GetHeight();
    }

    bool isHorScrollBarVisibled(){
        if (scrollBarVisibility == SCROLLBAR_AUTO) return isHorScrollable() && getContentWidth() > measuredWidth - totalPadding.GetWidth();
        return (scrollBarVisibility & SCROLLBAR_HORIZONTAL) && getContentWidth() > measuredWidth - totalPadding.GetWidth();
    }

    float getVerScrollBarLeft(){
        if (verScrollBarPosition == RIGHT){
            if ((scrollBarStyle & 2) != 0){     // outside
                return measuredWidth - getVerScrollBarSize();
            }
            else{
                return min(measuredWidth - getVerScrollBarSize(), measuredWidth - totalPadding.GetRight());
            }
        }
        else{
            if ((scrollBarStyle & 2) != 0){     // outside
                return 0;
            }
            else{
                return max(0.0f, totalPadding.GetLeft() - getVerScrollBarSize());
            }
        }
    }

    float getHorScrollBarTop(){
        if (verScrollBarPosition == BOTTOM){
            if ((scrollBarStyle & 2) != 0){     // outside
                return measuredHeight - getHorScrollBarSize();
            }
            else{
                return min(measuredHeight - getHorScrollBarSize(), measuredHeight - totalPadding.GetBottom());
            }
        }
        else{
            if ((scrollBarStyle & 2) != 0){     // outside
                return 0;
            }
            else{
                return max(0.0f, totalPadding.GetTop() - getHorScrollBarSize());
            }
        }
    }

    void update(float dt){
        if(dragging) return;

        const int threshold = 10;

        if (abs(vy) < threshold && (!ani || ani->isFinished())){
            vy = 0;
            float maxScrollY = max(0.0f, getContentHeight() - (measuredHeight - totalPadding.GetHeight()));
            if (scrollY < 0){
                ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollY"), 0, 0.3f);
                ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                ani->Play();
            }
            else if (scrollY > maxScrollY){
                ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollY"), maxScrollY, 0.3f);
                ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                ani->Play();
            }
        }

        if (abs(vx) < threshold && (!ani || ani->isFinished())){
            vx = 0;
            float maxScrollX = max(0.0f, getContentWidth() - (measuredWidth - totalPadding.GetWidth()));
            if (scrollX < 0){
                ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollX"), 0, 0.3f);
                ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                ani->Play();
            }
            else if (scrollX > maxScrollX){
                ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollX"), maxScrollX, 0.3f);
                ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                ani->Play();
            }
        }

        if (abs(vx) < threshold && abs(vy) < threshold){
            return;
        }

        if (isHorScrollBarVisibled() && horScrollThumb) { horScrollThumb->SetAlpha(1); horScrollThumb->FadeOut(0.5f, 0.8f); }
        if (isVerScrollBarVisibled() && verScrollThumb) { verScrollThumb->SetAlpha(1); verScrollThumb->FadeOut(0.5f, 0.8f); }
        if (isHorScrollBarVisibled() && horScrollTrack) { horScrollTrack->SetAlpha(1); horScrollTrack->FadeOut(0.5f, 0.8f); }
        if (isVerScrollBarVisibled() && verScrollTrack) { verScrollTrack->SetAlpha(1); verScrollTrack->FadeOut(0.5f, 0.8f); }
        float a = 100;
        float deg = atan2f(vy, vx);
        float ax = -a*cosf(deg);
        float ay = -a*sinf(deg);
        
        float clientWidth = measuredWidth - totalPadding.GetWidth();
        float clientHeight = measuredHeight - totalPadding.GetHeight();

        float maxScrollX = max(0.0f, getContentWidth() - clientWidth);
        float maxScrollY = max(0.0f, getContentHeight() - clientHeight);

        if (scrollX < 0){
            ax += sqrtf(scrollX / clientWidth) * 500;
        }
        else if (scrollX > maxScrollX){
            ax -= sqrtf((scrollX - maxScrollX) / clientWidth) * 500;
        }

        if (scrollY < 0){
            //ay += powf(scrollY / clientHeight, 2) * 500;
            vy -= vy*sqrtf(sqrtf(dt));
        }
        else if (scrollY > maxScrollY){
            vy -= vy*sqrtf(sqrtf(dt));
            //ay -= powf((scrollY - maxScrollY) / clientHeight, 2) * 500;
        }
        
        if (isHorScrollable()){
            //vx += ax*dt;
            vx -= vx * dt;
            scrollX += vx*dt;
        }
        if (isVerScrollable()){
            //vy += ay*dt;
            vy -= vy * dt * 2;
            scrollY += vy*dt;
        }
    }

public:
    ScrollView(){
        // error occurs here on gcc if set the second parameters to Color::Transparent
        verScrollThumb = make_shared<RectDrawable>(0x66666666, 0, 0, 3);
        horScrollThumb = make_shared<RectDrawable>(0x66666666, 0, 0, 3);
        verScrollThumb->SetAlpha(0);
        horScrollThumb->SetAlpha(0);
        scrollBarStyle = SCROLLBAR_INSIDE_INSETS;
        this->MouseDown += [this](View& v, MouseEventArgs e){
            if (e.Button != MouseButton::Left || !isScrollable()) return;
            dragging = true;
            down = e.Location();
            move1 = move2 = down;
            moveTime1 = moveTime2 = GetMicroSecond();
            vx = vy = 0;
            AnimationManager::Remove(ani);
            AnimationManager::Remove(ani2);
            ani = nullptr;
            ani2 = nullptr;
            if (isHorScrollBarVisibled() && horScrollThumb) horScrollThumb->FadeIn(0);
            if (isVerScrollBarVisibled() && verScrollThumb) verScrollThumb->FadeIn(0);
            if (isHorScrollBarVisibled() && horScrollTrack) horScrollTrack->FadeIn(0);
            if (isVerScrollBarVisibled() && verScrollTrack) verScrollTrack->FadeIn(0);
        };
        this->MouseMove += [this](View& v, MouseEventArgs e){
            if (e.Button != MouseButton::Left || !isScrollable()) return;
            if (isHorScrollable()) scrollX -= e.Location().X() - down.X();
            if (isVerScrollable()) scrollY -= e.Location().Y() - down.Y();
            down = e.Location();
            move1 = move2;
            move2 = down;
            moveTime1 = moveTime2;
            moveTime2 = GetMicroSecond();
        };
        this->MouseUp += [this](View& v, MouseEventArgs e){
            if (e.Button != MouseButton::Left || !isScrollable()) return;
            dragging = false;
            if (isHorScrollable()){
                float maxScrollX = max(0.0f, getContentWidth() - (measuredWidth - totalPadding.GetWidth()));
                if (scrollX < 0){
                    ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollX"), 0, 0.3f);
                    ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                    ani->Play();
                }
                else if (scrollX > maxScrollX){
                    ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollX"), maxScrollX, 0.3f);
                    ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                    ani->Play();
                }
                else{
                    float dt = (GetMicroSecond() - moveTime1) / 1000000.0f;
                    if (move1.X() != move2.X() && dt < 0.1f){
                        vx = -(move2.X() - move1.X()) / dt;
                        if (abs(vx) > getContentWidth()){
                            vx = getContentWidth() * (vx > 0 ? 1 : -1);
                        }
                    }
                }
                if (isHorScrollBarVisibled() && horScrollThumb){
                    horScrollThumb->FadeOut(0.5f, 0.8f);
                }
                if (isHorScrollBarVisibled() && horScrollTrack){
                    horScrollTrack->FadeOut(0.5f, 0.8f);
                }
            }
            if (isVerScrollable()){
                float maxScrollY = max(0.0f, getContentHeight() - (measuredHeight - totalPadding.GetHeight()));
                if (scrollY < 0){
                    ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollY"), 0, 0.3f);
                    ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                    ani->Play();
                }
                else if (scrollY > maxScrollY){
                    ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollY"), maxScrollY, 0.3f);
                    ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                    ani->Play();
                }
                else{
                    float dt = (GetMicroSecond() - moveTime1) / 1000000.0f;
                    if (move1.Y() != move2.Y() && dt < 0.1f){
                        vy = -(move2.Y() - move1.Y()) / dt;
                        if (abs(vy) > getContentHeight()){
                            vy = 500 * (vy > 0 ? 1 : -1);//getContentHeight() * (vy > 0 ? 1 : -1);
                        }
                    }
                }
                if (isVerScrollBarVisibled() && verScrollThumb){
                    verScrollThumb->FadeOut(0.5f, 0.8f);
                }
                if (isVerScrollBarVisibled() && verScrollTrack){
                    verScrollTrack->FadeOut(0.5f, 0.8f);
                }
            }
        };
    }

    class LayoutParams : public FrameLayout::LayoutParams{
        DECL_CLASS(ScrollView::LayoutParams);
    };

    virtual void OnMeasure(float width, float height, int widthMode, int heightMode) override{
        FrameLayout::OnMeasure(width, height, widthMode, heightMode);
        width = measuredWidth - totalPadding.GetWidth();
        height = measuredHeight - totalPadding.GetHeight();

        if (!views.empty()){
            auto view = views[0];
            auto param = static_pointer_cast<ScrollView::LayoutParams>(params[0]);
            float w = param->GetWidth();
            float h = param->GetHeight();
            view->Measure(GetChildMeasureSize(width, w, view->GetMargin().GetWidth()), GetChildMeasureSize(height, h, view->GetMargin().GetHeight()), param->GetWidth() == WRAP_CONTENT ? UNSPECIFIED : GetChildMeasureMode(widthMode, w), UNSPECIFIED);
        }

        if (isHorScrollBarVisibled()){
            float maxScrollX = getContentWidth() - (measuredWidth - totalPadding.GetWidth());
            if (scrollX < 0){
                ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollX"), 0, 0.3f);
                ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                ani->Play();
            }
            else if (scrollX > maxScrollX){
                ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollX"), maxScrollX, 0.3f);
                ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                ani->Play();
            }
        }
        if (isVerScrollBarVisibled()){
            float maxScrollY = getContentHeight() - (measuredHeight - totalPadding.GetHeight());
            if (scrollY < 0){
                ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollY"), 0, 0.3f);
                ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                ani->Play();
            }
            else if (scrollY > maxScrollY){
                ani = make_shared<ToAnimation<float>>(this, ScrollView::clazz.GetField("scrollY"), maxScrollY, 0.3f);
                ani->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
                ani->Play();
            }
        }
    }
    
    virtual void ResetTotalPadding() override{
        FrameLayout::ResetTotalPadding();
        if ((scrollBarStyle & 1) != 0){     // insets
            if (isVerScrollBarVisibled()){
                float vs = getVerScrollBarSize();
                if (verScrollBarPosition == RIGHT){
                    totalPadding = totalPadding + Padding(0, 0, vs, 0);
                }
                else{
                    totalPadding = totalPadding + Padding(vs, 0, 0, 0);
                }
            }

            if(isHorScrollBarVisibled()){
                float hs = getHorScrollBarSize();
                if (horScrollBarPosition == BOTTOM){
                    totalPadding = totalPadding + Padding(0, 0, 0, hs);
                }
                else{
                    totalPadding = totalPadding + Padding(0, hs, 0, 0);
                }
            }
        }
    }

    virtual void OnDrawFore(DrawEventArgs e) override{
        static int64_t t = 0;
        int64_t t2 = GetMicroSecond();
        
        if (t != 0){
            update((t2 - t) / 1000000.0f);
        }
        t = t2;

        Graphics& g = e.GetGraphics();

        g.Push();
        g.Translate(-scrollX, -scrollY);
        FrameLayout::OnDrawFore(e);
        g.Pop();
    }

    virtual void OnDrawOverlay(DrawEventArgs e) override{
        Graphics& g = e.GetGraphics();

        if (isVerScrollBarVisibled()){
            float size = getVerScrollBarSize();
            float left = getVerScrollBarLeft();
            if (verScrollTrack){
                float trackSize = max(scrollBarMinSize, verScrollTrack->GetSize().X());
                verScrollTrack->OnDraw(this, g, Rect(left + (size - trackSize) / 2,
                    totalPadding.GetTop(), size, measuredHeight - totalPadding.GetHeight()));
            }
            if (verScrollThumb){
                float thumbSize = max(scrollBarMinSize, verScrollThumb->GetSize().X());
                float clientHeight = measuredHeight - totalPadding.GetHeight();
                float contentHeight = getContentHeight();
                float maxScroll = contentHeight - clientHeight;
                if (scrollY < 0) contentHeight += -scrollY;
                if (scrollY > maxScroll) contentHeight += scrollY - maxScroll;
                float scrollY = max(0.0f, this->scrollY);
                float thumbLength = clientHeight * clientHeight / contentHeight;
                float thumbTop = totalPadding.GetTop() + scrollY / (contentHeight - clientHeight) * (1 - clientHeight / contentHeight) * clientHeight;
                verScrollThumb->OnDraw(this, g, Rect(left + (size - thumbSize) / 2,
                    thumbTop, size, thumbLength));
            }
        }

        if (isHorScrollBarVisibled()){
            float size = getHorScrollBarSize();
            float top = getHorScrollBarTop();
            if (horScrollTrack){
                float trackSize = max(scrollBarMinSize, verScrollTrack->GetSize().Y());
                horScrollTrack->OnDraw(this, g, Rect(totalPadding.GetLeft(),
                    top + (size - trackSize) / 2, measuredWidth - totalPadding.GetWidth(), size));
            }
            if (horScrollThumb){
                float thumbSize = max(scrollBarMinSize, verScrollThumb->GetSize().Y());
                float clientWidth = measuredWidth - totalPadding.GetWidth();
                float contentWidth = getContentWidth();
                if (scrollX < 0) contentWidth += -scrollX;
                float scrollX = max(0.0f, this->scrollX);
                float thumbLength = clientWidth * clientWidth / contentWidth;
                float thumbLeft = totalPadding.GetLeft() + scrollX / (contentWidth - clientWidth) * (1 - clientWidth / contentWidth) * contentWidth;
                horScrollThumb->OnDraw(this, g, Rect(totalPadding.GetTop(),
                    top + (size - thumbSize) / 2, thumbLength, size));
            }
        }
    }
};

class Scene{
protected:
    Rect bounds;
    shared_ptr<View> contentView;
    int isTranslucent = false;

public:
    bool IsTranslucent() const{
        return isTranslucent;
    }

    void SetTranslucent(bool isTranslucent){
        this->isTranslucent = isTranslucent;
    }

};

class Dialog{

};

class SceneManager{
private:
    stack<shared_ptr<Scene>> scenes;

public:
    void Push(shared_ptr<Scene> scene){
        scenes.push(scene);
    }

    shared_ptr<Scene> Pop(){
        auto top = scenes.top();
        scenes.pop();
        return top;
    }
};

class FontManager{
private:
    static vector<shared_ptr<Font>> fonts;
    static shared_ptr<Font> defaultFont;
    static unordered_map<string, string> fontFiles;
    static unordered_map<string, string> fontNames;

    static string trimSpaces(const string& str){
        vector<char> newStr;
        for (char c : str){
            if (c != ' '){
                newStr.push_back(c);
            }
        }
        newStr.push_back(0);
        return string(newStr.data());
    }

    static string GetFontPath(const string& fontName){
        if (!fontFiles[fontName].empty())
            return fontFiles[fontName];

        string file;
#ifdef WINDOWS
        file = GetSystemFontFile(fontName);
        if (file.empty()){
            file = GetSystemFontFile(getEnglishName(fontName));
        }
#elif defined ANDROID
        file = "/system/fonts/" + trimSpaces(fontName) + ".ttf";
#endif

        fontFiles[fontName] = file;

        return file;
    }

#ifdef WINDOWS

#define MAKE_TAG(a, b, c, d) (d<<24) | (c<<16) | (b<<8) | a
#define uchar unsigned char
#define uint32 unsigned int
#define uint16 unsigned short

    inline static uint16 getUShort(const uchar *table){
        return (*table << 8) | *(table + 1);
    }

    static string getEnglishName(const uchar *table, uint32 bytes);
    static string getEnglishName(const string &familyName);
    static string GetSystemFontFile(const string &faceName);

#endif

public:
    static void Init(){
#ifdef WINDOWS
        string fontName = "微软雅黑";
#elif defined ANDROID
        string fontName = "DroidSansFallback";
#endif
        defaultFont = CreateFont(fontName, Unit::ToPixel(12, Unit::UNIT_DP));
    }

    static shared_ptr<Font> GetDefaultFont() {
        return defaultFont;
    }

    static shared_ptr<Font> CreateFont(string fontName, float fontSize){
        if (fontName.empty()){
            fontName = defaultFont->GetName();
        }

        if (fontSize < 0){
            fontSize = Unit::ToPixel(12, Unit::UNIT_DP);
        }

        string file = GetFontPath(fontName);
        if (file.empty()){
            return nullptr;
        }

        shared_ptr<Font> font;
        string name = fontNames[file];
        if (name.empty()){
            font = make_shared<Font>(file, fontSize);
            name = font->GetName();
            fontNames[file] = name;
        }

        for (auto f : fonts){
            if (f->GetName() == name && f->GetSize() == fontSize){
                return f;
            }
        }

        if (font == nullptr){
            font = make_shared<Font>(file, fontSize);
        }
        fonts.push_back(font);
        return font;
    }
};

class Resources{
private:
    static std::unordered_map<std::string, shared_ptr<Texture>> textures;
    static std::unordered_map<std::string, shared_ptr<Drawable>> drawables;
    static std::unordered_map<std::string, shared_ptr<View>> views;

    static bool startWith(const string& s1, const string& s2){
        return s1.length() >= s2.length() && s1.substr(0, s2.length()) == s2;
    }

    static bool endWith(const string& s1, const string& s2){
        return s1.length() >= s2.length() && s1.substr(s1.length() - s2.length(), s2.length()) == s2;
    }

    static string to_upper(const string& s){
        int len = s.length();
        char* cs = new char[len+1];
        cs[len] = 0;
        for (int i = 0; i < len; i++){
            char c = s[i];
            if (islower(c)){
                c = c - 'a' + 'A';
            }
            else if (isupper(c)){
                c = c - 'A' + 'a';
            }
            cs[i] = c;
        }
        string r(cs);
        delete[] cs;
        return r;
    }

    static float read_float(Json& json){
        if (json.is_number()){
            return (float)json.number_value();
        }
        else if (json.is_string()){
            string s = to_upper((char*)json.string_value().c_str());
            float v = 0;
            if (isdigit(s[0]) || s[0] == '-'){
                sscanf(s.c_str(), "%f", &v);
                if (endWith(s, "DP"))
                    v = Unit::ToPixel(v, Unit::UNIT_DP);
                else if (endWith(s, "PT"))
                    v = Unit::ToPixel(v, Unit::UNIT_PT);
                else if (endWith(s, "MM"))
                    v = Unit::ToPixel(v, Unit::UNIT_MM);
                else if (endWith(s, "CM"))
                    v = Unit::ToPixel(v, Unit::UNIT_CM);
                else if (endWith(s, "IN"))
                    v = Unit::ToPixel(v, Unit::UNIT_IN);
                else
                    v = Unit::ToPixel(v, Unit::UNIT_PX);
            }
            else if (s == "WRAP_CONTENT") v = WRAP_CONTENT;
            else if (s == "MATCH_PARENT") v = MATCH_PARENT;
            return v;
        }
        return 0;
    }

    static void set_field(Object* obj, Field* f, Json& json){
        if (f->GetType()->GetName() == "float"){
            f->Set<float>(obj, read_float(json));
        }
        else if (f->GetType()->GetName() == "int"){
            if (json.is_number()){
                f->Set<int>(obj, (int)json.number_value());
            }
            else if (json.is_string()){
                string s = to_upper((char*)json.string_value().c_str());
                int v = 0;
                if (startWith(s, "#")){
                    v = Color::FromName(s);
                }
                else if (s == "CENTER") v = CENTER;
                else if (s == "LEFT") v = LEFT;
                else if (s == "TOP") v = TOP;
                else if (s == "RIGHT") v = RIGHT;
                else if (s == "BOTTOM") v = BOTTOM;
                else if (s == "CENTER_VERTICAL") v = CENTER_VERTICAL;
                else if (s == "CENTER_HORIZONTAL") v = CENTER_HORIZONTAL;
                else if (s == "HORIZONTAL") v = HORIZONTAL;
                else if (s == "VERTICAL") v = VERTICAL;
                else if (s == "NO_WRAP") v = FontFlag::FontFlag_NoWrap;
                else if (s == "INSIDE_OVERLAY") v = SCROLLBAR_INSIDE_OVERLAY;
                else if (s == "INSIDE_INSETS") v = SCROLLBAR_INSIDE_INSETS;
                else if (s == "OUTSIDE_OVERLAY") v = SCROLLBAR_OUTSIDE_OVERLAY;
                else if (s == "OUTSIDE_INSETS") v = SCROLLBAR_OUTSIDE_INSETS;
                else if (s == "SCROLLBAR_NONE") v = SCROLLBAR_NONE;
                else if (s == "SCROLLBAR_VERTICAL") v = SCROLLBAR_VERTICAL;
                else if (s == "SCROLLBAR_HORIZONTAL") v = SCROLLBAR_HORIZONTAL;
                else if (s == "SCROLLBAR_BOTH") v = SCROLLBAR_BOTH;
                else if (s == "SCROLLBAR_AUTO") v = SCROLLBAR_AUTO;
                /*else if (s == "LEFT") v = FontFlag::FontFlag_Left;
                else if (s == "TOP") v = FontFlag::FontFlag_Top;
                else if (s == "BOTTOM") v = FontFlag::FontFlag_Bottom;
                else if (s == "RIGHT") v = FontFlag::FontFlag_Right;
                else if (s == "CENTER") v = FontFlag::FontFlag_Center;
                else if (s == "MIDDLE") v = FontFlag::FontFlag_Middle;
                else if (s == "NOWRAP") v = FontFlag::FontFlag_NoWrap;*/
                f->Set<int>(obj, v);
            }
        }
        else if (f->GetType()->GetName() == "string"){
            if (json.is_string()){
                f->Set<string>(obj, json.string_value());
            }
        }
        else if (f->GetType()->GetName() == "bool"){
            if (json.is_bool()){
                f->Set<bool>(obj, json.bool_value());
            }
        }
        else if (!f->GetType()->IsDerivedFrom(&BaseType::clazz)){
            if (json.is_object()){
                f->Set(obj, f->GetType()->New());
                for (auto v : json.object_items()){
                    auto ff = f->GetType()->GetField(v.first);
                    set_field(f->Get(obj), ff, v.second);
                }
            }
            else if (json.is_string()){
                if (f->GetType()->GetName() == "Padding"){
                    float padding = read_float(json);
                    f->Set<Padding>(obj, Padding(padding));
                }
            }
        }
    }

public:
    static void ReleaseAll(){
        views.clear();
        drawables.clear();
        textures.clear();
    }

    static shared_ptr<Texture> LoadTexture(const std::string& file){
        auto tex = textures[file];
        if (tex){
            return tex;
        }

        string fullpath;
        if (FileSystem::GetInstance().Exists(fullpath = (file + ".png"))){
            tex = make_shared<Texture>(fullpath);
        }
        else if (FileSystem::GetInstance().Exists(fullpath = (file + ".bmp"))){
            tex = make_shared<Texture>(fullpath);
        }
        else if (FileSystem::GetInstance().Exists(fullpath = (file + ".9.png"))){
            tex = make_shared<NinePatchTexture>(fullpath);
        }
        else{
            return nullptr;
        }

        textures[file] = tex;
        //textures.insert(std::pair<std::string, shared_ptr<Texture>>(file, tex));
        return tex;
    }

    static shared_ptr<Drawable> LoadDrawable(Json& json){
        shared_ptr<Drawable> drawable;
        if (json.is_string()){
            string s = json.string_value();
            if (startWith(s, "#")){
                drawable = make_shared<ColorDrawable>(Color::FromName(s));
            }
            else if (startWith(s, "@")){
                drawable = Resources::LoadDrawable(s.substr(1));
            }
        }
        else if (json.is_object()){
            auto v = json.object_items();
            auto r = v.find("select");
            if (r != v.end()){
                if (r->second.is_array()){
                    drawable = make_shared<SelectDrawable>();
                    auto items = r->second.array_items();
                    for (auto& f : items){
                        State s;
                        for (auto& ff : f.object_items()){
                            if (ff.first == "drawable"){
                                s.SetDrawable(LoadDrawable((Json&)ff.second));
                            }
                            else{
                                void* value = nullptr;
                                if (ff.second.is_bool()){
                                    value = new bool(ff.second.bool_value());
                                }
                                else if (ff.second.is_number()){
                                    value = new float(ff.second.number_value());
                                }
                                else if (ff.second.is_string()){
                                    value = new string(ff.second.string_value());
                                }
                                s.Put(ff.first, value);
                            }
                        }
                        static_pointer_cast<SelectDrawable>(drawable)->AddState(s);
                    }
                }
            }
            else{
                string shape = v["shape"].string_value();
                if (!shape.empty()){
                    string className = string(1, toupper(shape[0])) + shape.substr(1) + "Drawable";
                    auto type = Class::ForName(className);
                    drawable = shared_ptr<Drawable>((Drawable*)type->New());
                    for (auto f : v){
                        if (f.first != "shape"){
                            set_field(drawable.get(), type->GetField(f.first), f.second);
                        }
                    }
                }
            }
        }

        return drawable;
    }

    static shared_ptr<View> LoadView(const string& file){
        string err;
        auto json = Json::parse(ReadTextFile(file), err);
        if (!err.empty()){
            DEBUG_LOG("failed to load \"%s\"\nerror: %s\n", file.c_str(), err.c_str());
            auto label = make_shared<Label>();
            label->SetText("failed to load " + file);
            label->SetColor(Color::Red);
            return label;
            //return nullptr;
        }
        return LoadView(json);
    }

    static shared_ptr<View> LoadView(Json& json){
        if (!json.is_object()){
            return nullptr;
        }
        auto items = json.object_items();
        auto clazz = items["class"];
        string name;
        if (!clazz.is_string() || (name = clazz.string_value()).empty()){
            return nullptr;
        }
        auto type = Class::ForName(name);
        shared_ptr<View> view((View*)type->New());
        for (auto p : items){
            string fieldName = p.first;
            if (fieldName != "class" && !startWith(fieldName, "layout_")){
                if (fieldName == "views"){
                    for (auto v : p.second.array_items()){
                        auto paramsType = Class::ForName(type->GetName() + "::LayoutParams");
                        Object* params = (Object*)paramsType->New();
                        for (auto f : v.object_items()){
                            string fn = f.first;
                            if (startWith(fn, "layout_")){
                                set_field(params, paramsType->GetField(fn.substr(7)), f.second);
                            }
                        }
                        ((ViewGroup*)view.get())->Add(LoadView(v), shared_ptr<LayoutParams>((LayoutParams*)params));
                    }
                }
                else if (type->GetField(fieldName)->GetType()->GetName() == "Drawable"){
                    auto drawable = LoadDrawable(p.second);

                    //*(shared_ptr<Drawable>*)type->GetField(fieldName)->Get(view.get()) = drawable;
                    type->GetField(fieldName)->Set<shared_ptr<Drawable>>(view.get(), drawable);
                }
                else if (type->GetField(fieldName)->GetType()->GetName() == "Font"){
                    string fontName;
                    float fontSize = -1;
                    for (auto f : p.second.object_items()){
                        string fn = f.first;
                        if (fn == "size"){
                            fontSize = read_float(f.second);
                        }
                        else if (fn == "name"){
                            fontName = f.second.string_value();
                        }
                    }
                    shared_ptr<Font> font = FontManager::CreateFont(fontName, fontSize);
                    static_pointer_cast<Label>(view)->SetFont(font.get());
                }
                else{
                    auto field = type->GetField(fieldName);
                    if (!field){
                        return nullptr;
                    }
                    set_field(view.get(), field, p.second);
                }
            }
        }

        string id = view->GetId();
        if (!id.empty()){
            if (views[id] != nullptr){
                DEBUG_LOG("warning: duplicated id \"%s\"\n", id.c_str());
            }
            else{
                views[id] = view;
            }
        }

        return view;
    }

    static shared_ptr<View> FindViewById(const string& id){
        return views[id];
    }

    static int UTF8ToUnicode(uint16_t *unicode, uint8_t *p, int bytes)
    {
        int e = 0, n = 0;
        if (p && unicode)
        {
            if (*p >= 0xf0)
            {
                if (bytes < 4)
                    return -1;
                /* 4:<11110000> */
                e = (p[0] & 0x07) << 18;
                e |= (p[1] & 0x3f) << 12;
                e |= (p[2] & 0x3f) << 6;
                e |= (p[3] & 0x3f);
                n = 4;
            }
            else if (*p >= 0xe0)
            {
                if (bytes < 3)
                    return -1;
                /* 3:<11100000> */
                e = (p[0] & 0x0f) << 12;
                e |= (p[1] & 0x3f) << 6;
                e |= (p[2] & 0x3f);
                n = 3;
            }
            else if (*p >= 0xc0)
            {
                if (bytes < 2)
                    return -1;
                /* 2:<11000000> */
                e = (p[0] & 0x1f) << 6;
                e |= (p[1] & 0x3f);
                n = 2;
            }
            else
            {
                if (bytes < 1)
                    return -1;
                e = p[0];
                n = 1;
            }
            *unicode = e;
        }
        /* Return bytes count of this utf-8 character */
        return n;
    }

    static int UTF8StrToUnicodeStr(uint16_t * unicode_str, uint8_t * utf8_str, int unicode_str_size){
        uint16_t unicode = 0;
        int n = 0;
        int count = 0;
        unsigned char *s = NULL;
        unsigned short *e = NULL;

        s = utf8_str;
        e = unicode_str;

        if ((utf8_str) && (unicode_str))
        {
            while (*s)
            {
                if ((n = UTF8ToUnicode(&unicode, s, 4)) > 0)
                {
                    if (++count >= unicode_str_size)
                    {
                        return count;
                    }
                    else
                    {
                        *e = (unsigned short)unicode;
                        e++;
                        *e = 0;

                        s += n;
                    }
                }
                else
                {
                    /* Converting error occurs */
                    return count;
                }
            }
        }

        return count;
    }

    static string ReadTextFile(const std::string& fileName){
        auto file = FileSystem::GetInstance().Open(fileName);
        int size = file->Size();
        char* buf = new char[size + 1];
        buf[size] = 0;
        file->Read(buf, size);

        string r;

#ifdef WINDOWS
        uint16_t* uni = new uint16_t[size + 1];
        UTF8StrToUnicodeStr(uni, (uint8_t*)buf, size + 1);
        uni[size] = 0;

        int len = WideCharToMultiByte(CP_ACP, NULL, (LPCWCH)uni, wcslen((LPCWCH)uni), NULL, 0, NULL, NULL);
        char* ansi = new char[len + 1];
        WideCharToMultiByte(CP_ACP, NULL, (LPCWCH)uni, wcslen((LPCWCH)uni), ansi, len, NULL, NULL);
        ansi[len] = 0;
        r = string(ansi);
        delete[] uni;
        delete[] ansi;
#elif defined ANDROID
        r = string(buf);
#endif
        delete[] buf;

        return r;
    }

    static shared_ptr<Drawable> LoadDrawable(const std::string& file){
        auto drawable = drawables[file];
        if (drawable){
            return drawable;
        }

        string fullpath;
        if (FileSystem::GetInstance().Exists(fullpath = (file + ".json"))){
            string s = ReadTextFile(fullpath);
            string err;
            auto json = Json::parse(s, err);
            drawable = LoadDrawable(json);
        }
        else if (FileSystem::GetInstance().Exists(fullpath = (file + ".png"))){
            drawable = make_shared<TextureDrawable>(LoadTexture(file));
        }
        else if (FileSystem::GetInstance().Exists(fullpath = (file + ".bmp"))){
            drawable = make_shared<TextureDrawable>(LoadTexture(file));
        }
        else if (FileSystem::GetInstance().Exists(fullpath = (file + ".9.png"))){
            drawable = make_shared<NinePatchTextureDrawable>(static_pointer_cast<NinePatchTexture>(LoadTexture(file)));
        }
        else{
            return nullptr;
        }

        drawables[file] = drawable;
        //drawables.insert(std::pair<std::string, shared_ptr<Drawable>>(file, drawable));
        return drawable;
    }
};


SE_END
