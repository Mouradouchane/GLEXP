
#include "main.hpp"

int APIENTRY wWinMain(
    _In_     HINSTANCE h_instance,
    _In_opt_ HINSTANCE h_prev_instance,
    _In_     LPWSTR    lp_cmd_line,
    _In_     int       n_cmd_show
){

    ASSERT(application::init());

    ASSERT(application::run());

    application::clean_up();
    return int(ERR::NO_ERR);
}
