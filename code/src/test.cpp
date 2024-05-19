// #include"StructDef.h"
// #include"PointInTriangle.h"
// int main() {
//     // 随机生成点和三角形
//     srand(time(nullptr));
//     Point point{static_cast<double>(rand()) / RAND_MAX, static_cast<double>(rand()) / RAND_MAX};
//     Triangle triangle = {
//         {static_cast<double>(rand()) / RAND_MAX, static_cast<double>(rand()) / RAND_MAX},
//         {static_cast<double>(rand()) / RAND_MAX, static_cast<double>(rand()) / RAND_MAX},
//         {static_cast<double>(rand()) / RAND_MAX, static_cast<double>(rand()) / RAND_MAX}
//     };
//     int count = 1E3;
//     int unit = 1E3;
//     bool resultArea,resultRay,resultBoundary;
//     // 测试面积法性能
//     clock_t start = clock();
//     for(int i=0;i<count;++i)
//         resultArea = isPointInTriangle_Area(point, triangle);
//     clock_t end = clock();
//     double elapsedTimeArea = (double(end - start) / CLOCKS_PER_SEC)/(count/unit);

//     // 测试射线法性能
//     start = clock();
//     for(int i=0;i<count;++i)
//         resultRay = isPointInTriangle_Ray(point, triangle);
//     end = clock();
//     double elapsedTimeRay = (double(end - start) / CLOCKS_PER_SEC)/(count/unit);

//     // 测试边界方程法性能
//     start = clock();
//     for(int i=0;i<count;++i)
//         resultBoundary = isPointInTriangle_EdgeFunction(point, triangle);
//     end = clock();
//     double elapsedTimeBoundary = (double(end - start) / CLOCKS_PER_SEC)/(count/unit);

//     // 输出结果
//     std::cout << "test" <<std::endl;
//     std::cout << "Point: (" << point.x << ", " << point.y << ")" << std::endl;
//     std::cout << "Triangle: (" << triangle.p1.x << ", " << triangle.p1.y << "), "
//               << "(" << triangle.p2.x << ", " << triangle.p2.y << "), "
//               << "(" << triangle.p3.x << ", " << triangle.p3.y << ")" << std::endl;
//     std::cout << "Is inside triangle (Area method): " << std::boolalpha << resultArea << ", Time: " << elapsedTimeArea << " ms" << std::endl;
//     std::cout << "Is inside triangle (Ray method): " << std::boolalpha << resultRay << ", Time: " << elapsedTimeRay << " ms" << std::endl;
//     std::cout << "Is inside triangle (Boundary method): " << std::boolalpha << resultBoundary << ", Time: " << elapsedTimeBoundary << " ms" << std::endl;

//     return 0;
// }
