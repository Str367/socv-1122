/****************************************************************************
  FileName     [ bddCmd.h ]
  PackageName  [ ]
  Synopsis     [ Define classes for BDD commands ]
  Author       [ Chung-Yang (Ric) Huang, Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2023-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BDD_CMD_H
#define BDD_CMD_H

#include "gvCmdMgr.h"

// ============================================================================
// Classes for BDD package commands
// ============================================================================
GV_COMMAND(BResetCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BSetVarCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BInvCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BAndCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BOrCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BNandCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BNorCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BXorCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BXnorCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BCofactorCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BExistCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BCompareCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BSimulateCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BReportCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BDrawCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BSetOrderCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(BConstructCmd, GV_CMD_TYPE_BDD);

GV_COMMAND(FSetVarCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FInvCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FAndCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FOrCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FNandCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FNorCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FXorCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FXnorCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FCofactorCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FExistCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FCompareCmd, GV_CMD_TYPE_BDD);
// GV_COMMAND(FSimulateCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FReportCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(FDrawCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(F2BCmd, GV_CMD_TYPE_BDD);
GV_COMMAND(B2FCmd, GV_CMD_TYPE_BDD);
// GV_COMMAND(FConstructCmd, GV_CMD_TYPE_BDD);

#endif  // BDD_CMD_H
