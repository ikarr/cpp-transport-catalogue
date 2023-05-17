#include "svg.h"

namespace svg {
using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
}

// ---------- Polyline

Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool first = true;
    for (const auto& point : points_) {
        if (first) {
            out << point.x << "," << point.y;
            first = false;
        } else {
            out << " " << point.x << "," << point.y;
        }   
    }
    out << "\""sv;
    RenderAttrs(out);
    out << "/>"sv;
}

// ---------- Text
    
Text::Text()
    : position_({0.0, 0.0})
    , offset_({0.0, 0.0})
    , font_size_(1)
    , font_family_("")
    , font_weight_("")
    , data_("") {}
    
Text& Text::SetPosition(Point pos) {
    position_ = pos;
    return *this;
}
 
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}
 
Text& Text::SetFontSize(uint32_t font_size) {
    font_size_ = font_size;
    return *this;
}
 
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = std::move(font_family);
    return *this;
}
 
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = std::move(font_weight);
    return *this;
}
 
Text& Text::SetData(std::string data) {
    data_ = std::move(data);
    ConvertToXML(data_);
    return *this;
}
 
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text x=\"" << position_.x << "\" y=\"" << position_.y;
    out << "\" dx=\"" << offset_.x << "\" dy=\"" << offset_.y;
    out << "\" font-size=\"" << font_size_;
    if (!font_family_.empty()) {
        out << "\" font-family=\"" << font_family_;
    }
    if (!font_weight_.empty()) {
        out << "\" font-weight=\"" << font_weight_;
    }
    out << "\"";
    RenderAttrs(out);
    out << ">" << data_ << "</text>";
}
    
void Text::ConvertToXML(std::string& data) const {
    size_t pos = 0;
    while ((pos = data.find('&', pos + 1)) != data.npos) {
        data.insert(pos + 1, "&amp;");
        data.erase(pos, 1);
    }
    while ((pos = data.find_first_of(R"("'<>)")) != data.npos) {
        switch (data[pos]) {
            case ('\"'):
                data.insert(pos + 1, "&quot;");
                break;
            case ('\''):
                data.insert(pos + 1, "&apos;");
                break;
            case ('<'):
                data.insert(pos + 1, "&lt;");
                break;
            case ('>'):
                data.insert(pos + 1, "&gt;");
                break;
        }
        data.erase(pos, 1);
    }
}
    
// ---------- Document

void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    if (obj) {
        objects_.emplace_back(std::move(obj));
    }
}
 
void Document::Render(std::ostream& out) const {
    RenderContext ctx(out, 2, 2);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    for (const auto& obj : objects_) {
        obj->Render(ctx);
    }
    out << "</svg>";
}

std::ostream& operator<<(std::ostream& output, StrokeLineCap slc) {
    switch (slc) {
        case svg::StrokeLineCap::BUTT:
            output << "butt"s;
            break;
        case svg::StrokeLineCap::ROUND:
            output << "round"s;
            break;
        case svg::StrokeLineCap::SQUARE:
            output << "square"s;
            break;
        default:
            break;
    }
    return output;
}

std::ostream& operator<<(std::ostream& output, StrokeLineJoin slj) {
    switch (slj) {
        case svg::StrokeLineJoin::ARCS:
            output << "arcs"s;
            break;
        case svg::StrokeLineJoin::BEVEL:
            output << "bevel"s;
            break;
        case svg::StrokeLineJoin::MITER:
            output << "miter"s;
            break;
        case svg::StrokeLineJoin::MITER_CLIP:
            output << "miter-clip"s;
            break;
        case svg::StrokeLineJoin::ROUND:
            output << "round"s;
            break;
        default:
            break;
    }
    return output;
}
    
} // namespace svg