/* Copyright (C) 1999  Dominik Vogt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <stdio.h>

#include "repeat.h"
#include "functions.h"
#include "parse.h"

/* If non-zero we are already repeating a function, so don't record the
 * command again. */
static unsigned int repeat_depth = 0;

/*
typedef struct
{
  char *start;
  char *end;
} double_ended_string;

static struct
{
  double_ended_string string;
  double_ended_string old;
  double_ended_string builtin;
  double_ended_string function;
  double_ended_string top_function;
  double_ended_string module;
  double_ended_string menu;
  double_ended_string popup;
  double_ended_string menu_or_popup;
  int page_x;
  int page_y;
  int desk;
  FvwmWindow *fvwm_window;
} last;
*/

static struct
{
  char *command;
} last = { NULL };

/*
char *repeat_last_function = NULL;
char *repeat_last_complex_function = NULL;
char *repeat_last_builtin_function = NULL;
char *repeat_last_module = NULL;
char *repeat_last_top_function = NULL;
char *repeat_last_menu = NULL;
FvwmWindow *repeat_last_fvwm_window = NULL;
*/

/* Prcedure: set_repeat_data
 *
 * Stores the contents of the data pointer internally for the repeat command.
 * The sype of data is determined by the 'type' parameter. If this function is
 * called to set a string value representing a fvwm builtin function the
 * 'func_typetion' can be set to the F_<func_type> value in the function table
 * in functions.c. If this value is set certain functions are not recorded.
 * The 'depth' parameter indicates the recursion depth of the current data
 * pointer (i.e. the first function call has a depth of one, functions called
 * from within this function have depth 2 and higher, this may be applicable
 * to future enhancements like menus).
 *
 *
 *
 *
 *
 */
Bool set_repeat_data(void *data, repeat_type type,
		     const struct functions *builtin)
{
  char **pdest;
  char *trash = NULL;

  /* No history recording during startup. */
  if (fFvwmInStartup)
    return True;

  switch(type)
  {
  case REPEAT_COMMAND:
    if (last.command == (char *)data)
      /* Already stored, no need to free the data pointer. */
      return False;
    if (data == NULL || repeat_depth != 0)
      /* Ignoring the data, must free it outside of this call. */
      return True;
    if (builtin && (builtin->flags & FUNC_DONT_REPEAT))
      /* Dont' record functions that have the FUNC_DONT_REPEAT flag set. */
      return True;
    if (last.command)
      free(last.command);
    /* Store a backup. */
    last.command = (char *)data;
    /* Since we stored the pointer the caller must not free it. */
    return False;
  case REPEAT_MENU:
  case REPEAT_POPUP:
  case REPEAT_PAGE:
  case REPEAT_DESK:
  case REPEAT_DESK_AND_PAGE:
  case REPEAT_FVWM_WINDOW:
  case REPEAT_NONE:
  default:
    return True;
  }
}

void repeat_function(F_CMD_ARGS)
{
  int index;
  char *optlist[] = {
    "command",
    NULL
  };

  repeat_depth++;
  /* Replay the backup, we don't want the repeat command recorded. */
  GetNextTokenIndex(action, optlist, 0, &index);
  switch (index)
  {
  case 0: /* command */
  default:
fprintf(stderr,"repeating 0x%x, %s\n",last.command,last.command);
    ExecuteFunction(last.command, tmp_win, eventp, context, *Module,
		    DONT_EXPAND_COMMAND);
    break;
  }
  repeat_depth--;
}
