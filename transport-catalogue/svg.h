#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace svg {

struct Rgb {
    Rgb() = default;

    Rgb(uint8_t red, uint8_t green, uint8_t blue)
        : red(red), green(green), blue(blue) {}

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba {
    Rgba() = default;

    Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
        : red(red), green(green), blue(blue), opacity(opacity) {}

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};
    
using Color = std::variant<std::monostate, Rgb, Rgba, std::string>;
inline const Color NoneColor{"none"};
    
struct ColorPrinter {
    void operator()(std::monostate) {
        using namespace std::literals;
        out << "none"sv;
    }

    void operator()(Rgb rgb) {
        using namespace std::literals;
        out << "rgb("sv;
        out << std::to_string(rgb.red) << ","sv;
        out << std::to_string(rgb.green) << ","sv;
        out << std::to_string(rgb.blue) << ")"sv;
    }

    void operator()(Rgba rgba) {
        using namespace std::literals;
        out << "rgba("sv;
        out << std::to_string(rgba.red) << ","sv;
        out << std::to_string(rgba.green) << ","sv;
        out << std::to_string(rgba.blue) << ","sv;
        out << rgba.opacity << ")"sv;
    }

    void operator()(std::string_view text) {
        out << text;
    }

    std::ostream& out;
    };

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};
    
inline std::ostream& operator<<(std::ostream& out, Color color) {
    using namespace std::literals;
    std::visit(ColorPrinter{out}, color);
    return out;
}

std::ostream& operator<<(std::ostream& output, StrokeLineCap slc);

std::ostream& operator<<(std::ostream& output, StrokeLineJoin slj);
    
struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    
    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }
    
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_linecap_ = std::move(line_cap);
        return AsOwner();
    }
    
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_linejoin_ = std::move(line_join);
        return AsOwner();
    }

protected:
    virtual ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;
        std::string_view pass = ""sv;
        fill_color_
            ? out << " fill=\""sv << *fill_color_ << "\""sv
            : out << pass;
        stroke_color_
            ? out << " stroke=\""sv << *stroke_color_ << "\""sv
            : out << pass;
        stroke_width_
            ? out << " stroke-width=\""sv << *stroke_width_ << "\""sv
            : out << pass;
        stroke_linecap_
            ? out << " stroke-linecap=\""sv << *stroke_linecap_ << "\""sv
            : out << pass;
        stroke_linejoin_
            ? out << " stroke-linejoin=\""sv << *stroke_linejoin_ << "\""sv
            : out << pass;
    }

private:
    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_linecap_;
    std::optional<StrokeLineJoin> stroke_linejoin_;
    
    Owner& AsOwner() {
    // static_cast безопасно преобразует *this к Owner&,
    // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }
};
    
// Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
// Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    Point center_;
    double radius_ = 1.0;
    
    void RenderObject(const RenderContext& context) const override;
};

class Polyline : public Object, public PathProps<Polyline> {
public:
    Polyline& AddPoint(Point point);

private:
    std::vector<Point> points_;
    
    void RenderObject(const RenderContext& context) const override;
};

class Text : public Object, public PathProps<Text> {
public:
    Text();
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

private:
    void RenderObject(const RenderContext& context) const override;
    void ConvertToXML(std::string& data) const;
 
    Point position_;
    Point offset_;
    uint32_t font_size_;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
};

class ObjectContainer {
public:
    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
};
    
class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;

    virtual ~Drawable() = default;
};

class Document : public ObjectContainer {
public:
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj);

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
};

} // namespace svg