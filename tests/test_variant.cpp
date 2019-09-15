#include <Variant.h>

int main()
{
    UA_Variant uavar;
    UA_Variant_init(&uavar);
    float f = 3.1;
    UA_Variant_setScalar(&uavar, &f, &UA_TYPES[UA_TYPES_FLOAT]);
    opc::Variant var{&uavar, false};
    float f2 = var.get<float>();

    //
    std::vector<float> fVector{1.1, 2.2, 3.3};
    var(fVector);

    std::vector<float> fVector2 = var.get<std::vector<float>>();

    // vector of bool ;) not working at the moment
    // std::vector<bool> .data is returning void
    // std::vector<bool> fMatrix{true, false, true};
    // var(fMatrix);

    std::array<bool, 3> boolArray{false, true, true};
    var(boolArray);

    return 0;
}