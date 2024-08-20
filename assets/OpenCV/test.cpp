#include <iostream>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point.hpp>

int main() {

    using namespace boost::geometry;
    typedef model::point<double, 2, cs::cartesian> point_t;
    typedef model::polygon<point_t> polygon_t;

    // 定义两个多边形
    polygon_t poly1, poly2;

    append(poly1, point_t(0.0, 0.0));
    append(poly1, point_t(0.0, -1.0));
    append(poly1, point_t(-1.0, -1.0));
    append(poly1, point_t(0.0, 0.0)); // 封闭多边形

    append(poly2, point_t(1.0, 1.0));
    append(poly2, point_t(3.0, 6.0));
    append(poly2, point_t(6.0, 6.0));
    append(poly2, point_t(1.0, 1.0)); // 封闭多边形

    // 判断两个多边形是否相交
    bool intersects = boost::geometry::intersects(poly1, poly2);
    std::cout << "Polygons intersect: " << intersects << std::endl;

    // 判断第一个多边形是否包含第二个多边形
    bool contains = boost::geometry::within(poly1, poly2);
    std::cout << "Polygon 1 contains Polygon 2: " << contains << std::endl;

    return 0;
}
