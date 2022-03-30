/**
 * \file            lwshell_opts.h
 * \brief           Template config file
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwSHELL - Lightweight shell library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.1
 */
#ifndef LWSHELL_HDR_OPTS_H
#define LWSHELL_HDR_OPTS_H

/**
 * \brief           Enables `1` or disables `0` output function to
 *                  print data from library to application.
 *
 * This is useful to give library feedback to user
 */
#define LWSHELL_CFG_USE_OUTPUT                  1

/**
 * \brief           Enables `1` or disables `0` generic ˙listcmd` command to list of registered commands
 *
 * \ref LWSHELL_CFG_USE_OUTPUT must be enabled to use this feature
 */
#define LWSHELL_CFG_USE_ENABLE_LIST_CMD         1


#endif /* LWSHELL_HDR_OPTS_H */
