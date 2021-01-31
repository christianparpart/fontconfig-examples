#include <fontconfig/fontconfig.h>

#include <cstdio>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

/// Gets given font and all its fallback fonts ordered by closeness to the input font properties.
vector<string> getFontList(string const& _family, bool _bold, bool _italic, bool _monospace, bool _color)
{
    auto pat = unique_ptr<FcPattern, void(*)(FcPattern*)>(
        FcPatternCreate(),
        [](auto p) { FcPatternDestroy(p); });

    FcPatternAddBool(pat.get(), FC_OUTLINE, true);
    FcPatternAddBool(pat.get(), FC_SCALABLE, true);

    if (_color)
        FcPatternAddBool(pat.get(), FC_COLOR, true);

    if (!_family.empty())
        FcPatternAddString(pat.get(), FC_FAMILY, (FcChar8 const*) _family.c_str());

    if (_monospace)
    {
        if (_family != "monospace")
            FcPatternAddString(pat.get(), FC_FAMILY, (FcChar8 const*) "monospace");
        FcPatternAddInteger(pat.get(), FC_SPACING, FC_DUAL);
    }

    if (_bold)
        FcPatternAddInteger(pat.get(), FC_WEIGHT, FC_WEIGHT_BOLD);

    if (_italic)
        FcPatternAddInteger(pat.get(), FC_SLANT, FC_SLANT_ITALIC);

    FcConfigSubstitute(nullptr, pat.get(), FcMatchPattern);
    FcDefaultSubstitute(pat.get());

    FcResult result = FcResultNoMatch;
    auto fs = unique_ptr<FcFontSet, void(*)(FcFontSet*)>(
        FcFontSort(nullptr, pat.get(), /*unicode-trim*/FcTrue, /*FcCharSet***/nullptr, &result),
        [](auto p) { FcFontSetDestroy(p); });

    if (!fs || result != FcResultMatch)
        return {};

    vector<string> output;

    for (int i = 0; i < fs->nfont; ++i)
    {
        FcPattern* font = fs->fonts[i];

        int spacing = -1;
        FcPatternGetInteger(font, FC_SPACING, 0, &spacing);

        FcChar8 *file;
        if (FcPatternGetString(font, FC_FILE, 0, &file) != FcResultMatch)
            continue;

        if (!_monospace || spacing >= FC_DUAL)
            output.emplace_back((char const*)(file));
    }

    return output;
}

int main(int argc, char const* argv[])
{
    FcInit();

    string family = argc >= 2 ? argv[1] : "Times New Roman";
    bool bold = false;
    bool italic = false;
    bool monospace = argc >= 3 && string_view(argv[2]) == "mono";

    auto fonts = getFontList(family, bold, italic, monospace, false);
    printf("count: %zu\n", fonts.size());
    for (auto const& font: fonts)
        printf("%s\n", font.c_str());

    FcFini();

    return 0;
}
