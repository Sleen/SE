#include "Vector.h"
#include "Rect.h"
#include "Type.h"
#include "View.h"


Class BaseType::clazz("BaseType", 0);

Class Object::clazz("Object", sizeof(Object), (Class::NEW_FUN)Object::Create);

IMPL_BASETYPE(int, Integer)
IMPL_BASETYPE(float, Float)
IMPL_BASETYPE(double, Double)
IMPL_BASETYPE(long, Long)
IMPL_BASETYPE(bool, Bool)
IMPL_BASETYPE(string, String)


Object* Field::Get(void* obj){
    //int size = type->GetSize();
    return (Object*)((char*)obj + offset);
}

void Field::Set(Object* obj, Object* value){
    if (value->GetClass() != type){
        return;
        //throw std::runtime_error("can not set a '" + value->GetClass()->GetName() + "' value to a '" + type->GetName() + "' field");
    }
    memcpy((char*)obj + offset, value, value->GetClass()->GetSize());
}

void* Class::CastFrom(Object* obj){
    return casts[obj->GetClass()](obj);
}

void* Class::CastFrom(int data){
    return casts[&Integer::clazz]((void*)data);
}


SE_BEGIN

Class Vector2::clazz("Vector2", nullptr, sizeof(Vector2), nullptr, 
    "x", offsetof(Vector2, data), &Float::clazz,
    "y", offsetof(Vector2, data) + sizeof(float), &Float::clazz, nullptr);
//IMPL_CLASS_WITH_FIELDS(Vector2, Object,
//    "x", offsetof(Vector2, data), &Float::clazz,
//    "y", offsetof(Vector2, data) + sizeof(float), &Float::clazz
//    );

IMPL_CLASS_WITH_FIELDS(Padding, Object,
    FIELD(Padding, left, Float),
    FIELD(Padding, top, Float),
    FIELD(Padding, right, Float),
    FIELD(Padding, bottom, Float)
    );

SE_END

namespace SEView{
    shared_ptr<RenderableTexture> alpha_buffer;

    shared_ptr<Curve> Curve::In()
    {
        return shared_ptr<Curve>(new InCurve(this->shared_from_this()));
    }
    shared_ptr<Curve> Curve::Out()
    {
        return this->shared_from_this();
    }
    shared_ptr<Curve> Curve::InOut()
    {
        return shared_ptr<Curve>(new InOutCurve(this->shared_from_this()));
    }
    shared_ptr<Curve> Curve::Repeat(float count)
    {
        return shared_ptr<Curve>(new RepeatCurve(this->shared_from_this(), count));
    }
    shared_ptr<Curve> Curve::Reverse()
    {
        return shared_ptr<Curve>(new ReverseCurve(this->shared_from_this()));
    }
    shared_ptr<Curve> Curve::operator+(shared_ptr<Curve> c)
    {
        return shared_ptr<Curve>(new CombineCurve(this->shared_from_this(), c));
    }
    shared_ptr<Curve> Curve::Merge(shared_ptr<Curve> another, float position)
    {
        return shared_ptr<Curve>(new MergeCurve(this->shared_from_this(), another, position));
    }

    /*template<typename T>
    T GetValue(T a, T b, float percent){
        return a + b * percent;
    }*/

    /*template<>
    int GetValue<int>(int a, int b, float percent){
        return a + b * percent;
    }

    template<>
    float GetValue<float>(float a, float b, float percent){
        return a + b * percent;
    }

    template<>
    double GetValue<double>(double a, double b, float percent){
        return a + b * percent;
    }

    template<>
    Vector2 GetValue<Vector2>(Vector2 a, Vector2 b, float percent){
        return a + b * percent;
    }*/

    list<shared_ptr<Animation>> AnimationManager::animations;

    void AnimationManager::Update(float dt){
        for (auto itr = animations.begin(); itr != animations.end(); ){
            auto ani = *itr;
            if (ani->Run(dt) >= 0){
                animations.erase(itr++);
            }
            else{
                itr++;
            }
        }
    }

    IMPL_ABS_CLASS_WITH_FIELDS(Drawable, Object,
        FIELD(Drawable, padding, Padding),
        FIELD(Drawable, size, Vector2),
        FIELD(Drawable, alpha, Float),
        "paddingLeft", offsetof(Drawable, padding) + Padding::clazz.GetField("left")->GetOffset(), &Float::clazz,
        "paddingTop", offsetof(Drawable, padding) + Padding::clazz.GetField("top")->GetOffset(), &Float::clazz,
        "paddingRight", offsetof(Drawable, padding) + Padding::clazz.GetField("right")->GetOffset(), &Float::clazz,
        "paddingBottom", offsetof(Drawable, padding) + Padding::clazz.GetField("bottom")->GetOffset(), &Float::clazz,
        "width", offsetof(Drawable, size) + Vector2::clazz.GetField("x")->GetOffset(), &Float::clazz,
        "height", offsetof(Drawable, size) + Vector2::clazz.GetField("y")->GetOffset(), &Float::clazz
        );

    void Drawable::FadeIn(float duration, float delay){
        AnimationManager::Remove(ani);
        auto seq = make_shared<Sequence>();
        auto a = make_shared<ToAnimation<float>>(this, Drawable::clazz.GetField("alpha"), 1, duration);
        a->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
        seq->Add(make_shared<Delay>(delay));
        seq->Add(a);
        seq->Play();
        ani = seq;
    }

    void Drawable::FadeOut(float duration, float delay){
        AnimationManager::Remove(ani);
        auto seq = make_shared<Sequence>();
        auto a = make_shared<ToAnimation<float>>(this, Drawable::clazz.GetField("alpha"), 0, duration);
        a->SetCurve(make_shared<BezierCurve>(0.5, 0, 0, 1));
        seq->Add(make_shared<Delay>(delay));
        seq->Add(a);
        seq->Play();
        ani = seq;
    }

    IMPL_CLASS_WITH_FIELDS(ColorDrawable, Drawable,
        FIELD(ColorDrawable, color, Integer)
        );

    IMPL_ABS_CLASS_WITH_FIELDS(ShapeDrawable, Drawable,
        FIELD(ShapeDrawable, solidColor, Integer),
        FIELD(ShapeDrawable, strokeColor, Integer),
        FIELD(ShapeDrawable, strokeWidth, Float)
        );

    IMPL_CLASS_WITH_FIELDS(RectDrawable, ShapeDrawable,
        FIELD(RectDrawable, radius, Float),
        FIELD(RectDrawable, corner, Integer)
        );

    IMPL_CLASS(OvalDrawable, ShapeDrawable);

    IMPL_CLASS_WITH_FIELDS(LineDrawable, Drawable,
        FIELD(LineDrawable, width, Float),
        FIELD(LineDrawable, direction, Integer),
        FIELD(LineDrawable, color, Integer)
        );

    IMPL_CLASS(SelectDrawable, Drawable);

    IMPL_CLASS_WITH_FIELDS(ViewGroup::LayoutParams, Object,
        FIELD(ViewGroup::LayoutParams, width, Float),
        FIELD(ViewGroup::LayoutParams, height, Float)
        );

    IMPL_CLASS_WITH_FIELDS(LinearLayout::LayoutParams, ViewGroup::LayoutParams,
        FIELD(LinearLayout::LayoutParams, weight, Float),
        FIELD(LinearLayout::LayoutParams, gravity, Integer)
        );

    IMPL_CLASS_WITH_FIELDS(FrameLayout::LayoutParams, ViewGroup::LayoutParams,
        FIELD(FrameLayout::LayoutParams, gravity, Integer)
        );

    IMPL_CLASS(ScrollView::LayoutParams, FrameLayout::LayoutParams);

    IMPL_CLASS_WITH_FIELDS(View, Object, 
        FIELD(View, padding, Padding),
        FIELD(View, margin, Padding),
        FIELD(View, background, Drawable), 
        FIELD(View, id, String),
        FIELD(View, scale, Vector2),
        FIELD(View, translate, Vector2),
        FIELD(View, center, Vector2),
        FIELD(View, rotate, Float),
        FIELD(View, alpha, Float),
        FIELD(View, visibled, Bool),
        FIELD(View, enabled, Bool),
        FIELD(View, mouseDown, Bool),
        FIELD(View, mouseIn, Bool),
        "scaleX", offsetof(View, scale) + Vector2::clazz.GetField("x")->GetOffset(), &Float::clazz,
        "scaleY", offsetof(View, scale) + Vector2::clazz.GetField("y")->GetOffset(), &Float::clazz,
        "translateX", offsetof(View, translate) + Vector2::clazz.GetField("x")->GetOffset(), &Float::clazz,
        "translateY", offsetof(View, translate) + Vector2::clazz.GetField("y")->GetOffset(), &Float::clazz,
        "paddingLeft", offsetof(View, padding) + Padding::clazz.GetField("left")->GetOffset(), &Float::clazz,
        "paddingTop", offsetof(View, padding) + Padding::clazz.GetField("top")->GetOffset(), &Float::clazz,
        "paddingRight", offsetof(View, padding) + Padding::clazz.GetField("right")->GetOffset(), &Float::clazz,
        "paddingBottom", offsetof(View, padding) + Padding::clazz.GetField("bottom")->GetOffset(), &Float::clazz,
        "marginLeft", offsetof(View, margin) + Padding::clazz.GetField("left")->GetOffset(), &Float::clazz,
        "marginTop", offsetof(View, margin) + Padding::clazz.GetField("top")->GetOffset(), &Float::clazz,
        "marginRight", offsetof(View, margin) + Padding::clazz.GetField("right")->GetOffset(), &Float::clazz,
        "marginBottom", offsetof(View, margin) + Padding::clazz.GetField("bottom")->GetOffset(), &Float::clazz
        );

    IMPL_CLASS(ViewGroup, View);
    IMPL_CLASS_WITH_FIELDS(LinearLayout, ViewGroup,
        FIELD(LinearLayout, orientation, Integer),
        FIELD(LinearLayout, dividers, Integer),
        FIELD(LinearLayout, dividerDrawable, Drawable)
        );
    IMPL_CLASS(FrameLayout, ViewGroup);

    IMPL_CLASS_WITH_FIELDS(ScrollView, FrameLayout,
        FIELD(ScrollView, scrollX, Float),
        FIELD(ScrollView, scrollY, Float),
        FIELD(ScrollView, verScrollTrack, Drawable),
        FIELD(ScrollView, horScrollTrack, Drawable),
        FIELD(ScrollView, verScrollThumb, Drawable),
        FIELD(ScrollView, horScrollThumb, Drawable),
        FIELD(ScrollView, verScrollBarPosition, Integer),
        FIELD(ScrollView, horScrollBarPosition, Integer),
        FIELD(ScrollView, scrollBarStyle, Integer),
        FIELD(ScrollView, scrollBarVisibility, Integer),
        FIELD(ScrollView, scrollability, Integer));

    Class font_class("Font", sizeof(Font));

    IMPL_CLASS_WITH_FIELDS(Label, View,
        FIELD(Label, text, String),
        FIELD(Label, color, Integer),
        FIELD(Label, flags, Integer),
        "font", offsetof(Label, font), &font_class
        );

    IMPL_CLASS_WITH_FIELDS(ImageView, View,
        FIELD(ImageView, drawable, Drawable)
        );

    int View::GetDepth(){
        return (!bufferring && parent) ? parent->GetDepth() + 1 : 1;
    }

    std::unordered_map<std::string, shared_ptr<Texture>> Resources::textures;
    std::unordered_map<std::string, shared_ptr<Drawable>> Resources::drawables;
    std::unordered_map<std::string, shared_ptr<View>> Resources::views;
    
    vector<shared_ptr<Font>> FontManager::fonts;
    shared_ptr<Font> FontManager::defaultFont;
    unordered_map<string, string> FontManager::fontFiles;
    unordered_map<string, string> FontManager::fontNames;

#ifdef WINDOWS

    string FontManager::GetSystemFontFile(const string &faceName){

        static const LPSTR fontRegistryPath = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
        HKEY hKey;
        LONG result;
        std::string wsFaceName(faceName.begin(), faceName.end());

        // Open Windows font registry key
        result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey);
        if (result != ERROR_SUCCESS) {
            return "";
        }

        DWORD maxValueNameSize, maxValueDataSize;
        result = RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
        if (result != ERROR_SUCCESS) {
            return "";
        }

        DWORD valueIndex = 0;
        LPSTR valueName = new CHAR[maxValueNameSize];
        LPBYTE valueData = new BYTE[maxValueDataSize];
        DWORD valueNameSize, valueDataSize, valueType;
        std::string wsFontFile;

        // Look for a matching font name
        do {

            wsFontFile.clear();
            valueDataSize = maxValueDataSize;
            valueNameSize = maxValueNameSize;

            result = RegEnumValue(hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize);

            valueIndex++;

            if (result != ERROR_SUCCESS || valueType != REG_SZ) {
                continue;
            }

            std::string wsValueName(valueName, valueNameSize);

            // Found a match
            if (_strnicmp(wsFaceName.c_str(), wsValueName.c_str(), wsFaceName.length()) == 0) {

                wsFontFile.assign((LPSTR)valueData, valueDataSize);
                break;
            }
        } while (result != ERROR_NO_MORE_ITEMS);

        delete[] valueName;
        delete[] valueData;

        RegCloseKey(hKey);

        if (wsFontFile.empty()) {
            return "";
        }

        // Build full font file path
        CHAR winDir[MAX_PATH];
        GetWindowsDirectory(winDir, MAX_PATH);

        std::stringstream ss;
        ss << winDir << "\\Fonts\\" << wsFontFile;
        wsFontFile = ss.str();

        return std::string(wsFontFile.begin(), wsFontFile.end());
    }

    string FontManager::getEnglishName(const uchar *table, uint32 bytes)
    {
        string i18n_name;
        enum {
            NameRecordSize = 12,
            FamilyId = 1,
            MS_LangIdEnglish = 0x009
        };

        // get the name table
        uint16 count;
        uint16 string_offset;
        const unsigned char *names;

        int microsoft_id = -1;
        int apple_id = -1;
        int unicode_id = -1;

        if (getUShort(table) != 0)
            goto error;

        count = getUShort(table + 2);
        string_offset = getUShort(table + 4);
        names = table + 6;

        if (string_offset >= bytes || 6 + count*NameRecordSize > string_offset)
            goto error;

        for (int i = 0; i < count; ++i) {
            // search for the correct name entry

            uint16 platform_id = getUShort(names + i*NameRecordSize);
            uint16 encoding_id = getUShort(names + 2 + i*NameRecordSize);
            uint16 language_id = getUShort(names + 4 + i*NameRecordSize);
            uint16 name_id = getUShort(names + 6 + i*NameRecordSize);

            if (name_id != FamilyId)
                continue;

            enum {
                PlatformId_Unicode = 0,
                PlatformId_Apple = 1,
                PlatformId_Microsoft = 3
            };

            uint16 length = getUShort(names + 8 + i*NameRecordSize);
            uint16 offset = getUShort(names + 10 + i*NameRecordSize);
            if (DWORD(string_offset + offset + length) >= bytes)
                continue;

            if ((platform_id == PlatformId_Microsoft
                && (encoding_id == 0 || encoding_id == 1))
                && (language_id & 0x3ff) == MS_LangIdEnglish
                && microsoft_id == -1)
                microsoft_id = i;
            // not sure if encoding id 4 for Unicode is utf16 or ucs4...
            else if (platform_id == PlatformId_Unicode && encoding_id < 4 && unicode_id == -1)
                unicode_id = i;
            else if (platform_id == PlatformId_Apple && encoding_id == 0 && language_id == 0)
                apple_id = i;
        }
        {
            bool unicode = false;
            int id = -1;
            if (microsoft_id != -1) {
                id = microsoft_id;
                unicode = true;
            }
            else if (apple_id != -1) {
                id = apple_id;
                unicode = false;
            }
            else if (unicode_id != -1) {
                id = unicode_id;
                unicode = true;
            }
            if (id != -1) {
                uint16 length = getUShort(names + 8 + id*NameRecordSize);
                uint16 offset = getUShort(names + 10 + id*NameRecordSize);
                if (unicode) {
                    // utf16

                    length /= 2;
                    i18n_name.resize(length);
                    char *uc = (char*)i18n_name.c_str();
                    const unsigned char *string = table + string_offset + offset;
                    for (int i = 0; i < length; ++i)
                        uc[i] = getUShort(string + 2 * i);
                }
                else {
                    // Apple Roman

                    i18n_name.resize(length);
                    char *uc = (char *)i18n_name.c_str();
                    const unsigned char *string = table + string_offset + offset;
                    for (int i = 0; i < length; ++i)
                        uc[i] = string[i];
                }
            }
        }
    error:
        //qDebug("got i18n name of '%s' for font '%s'", i18n_name.latin1(), familyName.toLocal8Bit().data());
        return i18n_name;
    }

    string FontManager::getEnglishName(const string &familyName)
    {
        string i18n_name;

        HDC hdc = GetDC(0);
        LOGFONT lf;
        memset(&lf, 0, sizeof(LOGFONT));
        memcpy(lf.lfFaceName, familyName.c_str(), min(LF_FACESIZE, familyName.length()) * sizeof(char));
        lf.lfCharSet = DEFAULT_CHARSET;
        HFONT hfont = CreateFontIndirect(&lf);

        if (!hfont) {
            ReleaseDC(0, hdc);
            return "";
        }

        HGDIOBJ oldobj = SelectObject(hdc, hfont);

        const DWORD name_tag = MAKE_TAG('n', 'a', 'm', 'e');

        // get the name table
        unsigned char *table = 0;

        DWORD bytes = GetFontData(hdc, name_tag, 0, 0, 0);
        if (bytes == GDI_ERROR) {
            // ### Unused variable
            /* int err = GetLastError(); */
            goto error;
        }

        table = new unsigned char[bytes];
        GetFontData(hdc, name_tag, 0, table, bytes);
        if (bytes == GDI_ERROR)
            goto error;

        i18n_name = getEnglishName(table, bytes);
    error:
        delete[] table;
        SelectObject(hdc, oldobj);
        DeleteObject(hfont);
        ReleaseDC(0, hdc);

        //qDebug("got i18n name of '%s' for font '%s'", i18n_name.latin1(), familyName.toLocal8Bit().data());
        return i18n_name;
    }

#endif

}
