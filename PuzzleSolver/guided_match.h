#ifndef EDIT_CORNERS_H
#define EDIT_CORNERS_H

#include "piece.h"
#include "params.h"

#define GM_COMMAND_X_CLOSE "x_close"
#define GM_COMMAND_NO "no"
#define GM_COMMAND_YES "yes"
#define GM_COMMAND_SHOW_SET "show_set"
#define GM_COMMAND_SHOW_ROTATION "show_rotation"
#define GM_COMMAND_WORK_ON_SET "work_on_set"
#define GM_COMMAND_MARK_BOUNDARY "mark_boundary"


std::string guided_match(piece& p1, piece& p2, int e1, int e2, params& user_params);

#endif /* EDIT_CORNERS_H */

