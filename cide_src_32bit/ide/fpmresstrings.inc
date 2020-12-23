{
    This file is part of the Free Pascal Integrated Development Environment
    Copyright (c) 1998 by Berczi Gabor

    Constants used by the IDE

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    
 ----------------------------------------------------------------------   
    
    ==> diese Datei ist KEINE Originaldatei von FP und FPIDE. Die
    Definitionen sind dem Author bei der Modifizierung der originalen
    Datei einfach zu lang und macht seiner Ansicht nach den Quellcode 
    unübersichtlich!
    
    03.01.2015    R. Seelig
 **********************************************************************}


resourcestring  menu_local_gotosource = '~G~oto source';
                menu_local_tracksource = '~T~rack source';
                menu_local_options = '~O~ptions...';
                menu_local_clear = '~C~lear';
                menu_local_saveas = 'Save ~a~s';


{                menu_key_common_helpindex      = 'Shift+F1';
                menu_key_common_topicsearch    = 'Ctrl+F1';
                menu_key_common_prevtopic      = 'Alt+F1';}

                { menu entries }
                menu_file              = '~F~ile';
                menu_file_new          = '~N~ew';
                menu_file_template     = 'New from ~t~emplate...';
                menu_file_open         = '~O~pen...';
                menu_file_save         = '~S~ave';
                menu_file_saveas       = 'Save ~a~s...';
                menu_file_saveall      = 'Save a~l~l';
                menu_file_reload       = '~R~eload';
                menu_file_print        = '~P~rint';
                menu_file_printsetup   = 'Print s~e~tup';
                menu_file_changedir    = '~C~hange dir...';
                menu_file_dosshell     = 'Comman~d~ shell';
                menu_file_exit         = 'E~x~it';

                menu_edit              = '~E~dit';
                menu_edit_copywin      = 'Cop~y~ to Windows';
                menu_edit_pastewin     = 'Paste from ~W~indows';
                menu_edit_undo         = '~U~ndo';
                menu_edit_redo         = '~R~edo';
                menu_edit_cut          = 'Cu~t~';
                menu_edit_copy         = '~C~opy';
                menu_edit_paste        = '~P~aste';
                menu_edit_clear        = 'C~l~ear';
                menu_edit_showclipboard= '~S~how clipboard';
                menu_edit_selectall    = 'Select ~A~ll';
                menu_edit_unselect     = 'U~n~select';

                menu_search            = '~S~earch';
                menu_search_find       = '~F~ind...';
                menu_search_replace    = '~R~eplace...';
                menu_search_searchagain= '~S~earch again';
                menu_search_jumpline   = '~G~o to line number...';
                menu_search_findproc   = 'Find ~p~rocedure...';
                menu_search_objects    = '~O~bjects';
                menu_search_modules    = 'Mod~u~les';
                menu_search_globals    = 'G~l~obals';
                menu_search_symbol     = 'S~y~mbol...';

                menu_run               = '~R~un';
                menu_run_run           = '~R~un';
                menu_run_continue      = '~C~ontinue';
                menu_run_stepover      = '~S~tep over';
                menu_run_traceinto     = '~T~race into';
                menu_run_conttocursor  = '~G~oto Cursor';
                menu_run_untilreturn   = '~U~ntil return';
                menu_run_rundir        = 'Run ~D~irectory...';
                menu_run_parameters    = 'P~a~rameters...';
                menu_run_resetdebugger = '~P~rogram reset';

                menu_compile           = '~C~ompile';
                menu_compile_compile   = '~C~ompile';
                menu_compile_build     = '~U~pload';
                menu_compile_target    = '~T~arget...';
                menu_compile_primaryfile = '~P~rimary file...';
                menu_compile_clearprimaryfile = 'C~l~ear primary file';
                menu_compile_information = '~I~nformation...';
                menu_compile_compilermessages = 'Compiler messages';

                menu_tools             = '~T~ools';
                menu_tools_messages    = '~M~essages';
                menu_tools_msgnext     = 'Goto ~n~ext';
                menu_tools_msgprev     = 'Goto ~p~revious';
                menu_tools_grep        = '~G~rep';
                menu_tools_calculator  = '~C~alculator';
                menu_tools_asciitable  = 'Ascii ~t~able';

                menu_options_env_preferences = '~P~references...';
                menu_options_env_editor= '~E~ditor...';
                menu_options_env_keybmouse = 'Keyboard & ~m~ouse...';

                menu_window            = '~W~indows';
                menu_window_tile       = '~T~ile';
                menu_window_cascade    = 'C~a~scade';
                menu_window_closeall   = 'Cl~o~se all';
                menu_window_resize     = '~S~ize/Move';
                menu_window_zoom       = '~Z~oom';
                menu_window_next       = '~N~ext';
                menu_window_previous   = '~P~revious';
                menu_window_hide       = '~H~ide';
                menu_window_close      = '~C~lose';
                menu_window_list       = '~L~ist...';
                menu_window_update     = '~R~efresh display';

                { short cut entries in menu }
                menu_key_file_open     = 'F3';
                menu_key_file_save     = 'F2';
                menu_key_file_exit     = 'Alt+X';

                menu_key_edit_undo     = 'Alt+BkSp';
                menu_key_edit_cut_borland      = 'Shift+Del';
                menu_key_edit_copy_borland     = menu_key_common_copy_borland;
                menu_key_edit_paste_borland    = 'Shift+Ins';
                menu_key_edit_cut_microsoft    = 'Ctrl+X';
                menu_key_edit_copy_microsoft   = menu_key_common_copy_microsoft;
                menu_key_edit_paste_microsoft  = 'Ctrl+V';
                menu_key_edit_clear    = 'Ctrl+Del';

                menu_key_run_run       = 'Ctrl+F9';
                menu_key_run_stepover  = 'F8';
                menu_key_run_traceinto = 'F7';
                menu_key_run_conttocursor = 'F4';
                menu_key_run_untilreturn= 'Alt+F4';
                menu_key_run_resetdebugger = 'Ctrl+F2';

                menu_key_compile_compile = 'Alt+F9';
                menu_key_compile_make = 'F9';
                menu_key_compile_compilermessages = 'F12';

                menu_key_debug_userscreen = 'Alt+F5';
                menu_key_debug_breakpoint = 'Ctrl+F8';
                menu_key_debug_callstack = 'Ctrl+F3';
                menu_key_debug_addwatch = 'Ctrl+F7';

                menu_key_tools_messages= 'F11';
                menu_key_tools_msgnext = 'Alt+F8';
                menu_key_tools_msgprev = 'Alt+F7';
                menu_key_tools_grep    = 'Shift+F2';

                menu_key_window_resize = 'Ctrl+F5';
                menu_key_window_zoom   = 'F5';
                menu_key_window_next   = 'F6';
                menu_key_window_previous = 'Shift+F6';
                menu_key_window_close  = 'Alt+F3';
                menu_key_window_list   = 'Alt+0';
                menu_key_window_hide   = 'Ctrl+F6';

                menu_help              = '~H~elp';
                menu_help_contents     = '~C~ontents';
                menu_help_index        = '~I~ndex';
                menu_help_topicsearch  = '~T~opic search';
                menu_help_prevtopic    = '~P~revious topic';
                menu_help_using        = '~U~sing help';
                menu_help_files        = '~F~iles...';
                menu_help_about        = '~A~bout...';

                menu_key_hlplocal_index = menu_key_common_helpindex;
                menu_key_hlplocal_topicsearch = menu_key_common_topicsearch;
                menu_key_hlplocal_prevtopic = menu_key_common_prevtopic;
                menu_key_hlplocal_copy_borland = menu_key_common_copy_borland;
                menu_key_hlplocal_copy_microsoft = menu_key_common_copy_microsoft;

                { status line entries }
                status_help            = '~F1~ Help';
                status_help_on_help    = '~F1~ Help on help';
                status_help_previoustopic = '~Alt+F1~ Previous topic';
                status_help_index      = '~Shift+F1~ Help index';
                status_help_close      = '~Esc~ Close help';
                status_save            = '~F2~ Save';
                status_open            = '~F3~ Open';
                status_compile         = '~Alt+F9~ Compile';
                status_make            = '~F9~ Make';
                status_localmenu       = '~Alt+F10~ Local menu';
                status_transferchar    = '~Ctrl+Enter~ Transfer char';
                status_msggotosource   = '~'+EnterSign+'~ Goto source';
                status_msgtracksource  = '~Space~ Track source';
                status_close           = '~Esc~ Close';
                status_calculatorpaste = '~Ctrl+Enter~ Transfer result';
                status_disassemble     = '~Alt+I~ Disassemble';

                { error messages }
                error_saving_cfg_file  = 'Error saving configuration.';
                error_saving_dsk_file  = 'Error saving desktop file.'#13+
                                         'Desktop layout could not be stored.';
                error_user_screen_not_avail = 'Sorry, user screen not available.';

                { standard button texts }
                button_OK          = 'O~K~';
                button_Cancel      = 'Cancel';
                button_New         = '~N~ew';
                button_Delete      = '~D~elete';
                button_Show        = '~S~how';
                button_Hide        = '~H~ide';

                { dialogs }
                dialog_fillintemplateparameter = 'Fill in template parameter';
                dialog_calculator       = 'Calculator';
                dialog_openafile        = 'Open a file';
                dialog_browsesymbol = 'Browse Symbol';

                msg_confirmsourcediradd = 'Directory %s is not in search path for source files. '+
                                         'Should we add it ?';
                msg_quitconfirm         = 'Do You really want to quit?';
                msg_printernotopened = 'Can''t open printer,'#13#3'check device name in "print setup"';
                msg_printerror = 'Error while printing';
                msg_impossibletoreachcursor = 'Impossible to reach current cursor';
                msg_impossibletosetbreakpoint = 'Impossible to set breakpoints here';
                msg_nothingtorun = 'Oooops, nothing to run.';
                msg_cannotrununit = 'Can''t run a unit';
                msg_cannotrunlibrary = 'Can''t run a library';
                msg_errorexecutingshell = 'Error cannot run shell';

                msg_userscreennotavailable = 'Sorry, user screen not available.';
                msg_cantsetscreenmode = #3'Impossible to set'#13#3'%dx%d mode';
                msg_confirmnewscreenmode = 'Please, confirm that new mode'#13 +
                                           'is displayed correctly';

                { Debugger confirmations and messages }
                msg_nodebuginfoavailable = 'No debug info available.';
                msg_nodebuggersupportavailable = 'No debugger support available.';

                msg_invalidfilename = 'Invalid filename %s';

                { File|New from template dialog }
                msg_notemplatesavailable = 'No templates available.';
                dialog_newfromtemplate   = 'New from template';
                label_availabletemplates = 'Available ~t~emplates';

                label_filetoopen        = 'File to ope~n~';
                label_lookingfor        = 'Looking for %s';

                {Printing.}
                dialog_setupprinter = 'Setup printer';
                label_setupprinter_device = '~D~evice';

                {Find procedure.}
                dialog_proceduredialog = 'Find Procedure';
                label_enterproceduretofind = 'Enter ~m~atching expr.';
                label_sym_findprocedure = 'Procedures';
                label_sym_findprocedure2 = 'Matching ';

                { Browser messages }
{                msg_symbolnotfound = #3'Symbol %s not found';
                msg_nobrowserinfoavailable = 'No Browser info available';}
                msg_toomanysymbolscantdisplayall= 'Too many symbols. Can''t display all of them.';

                label_sym_objects = 'Objects';
                label_sym_globalscope = 'Global scope';
                label_sym_globals = 'Globals';

                dialog_units = 'Units';

                label_entersymboltobrowse = 'Enter S~y~mbol to browse';

                {Program parameters dialog.}
                dialog_programparameters = 'Program parameters';
                label_parameters_parameter = '~P~arameter';
                msg_programnotrundoserroris = #3'Program %s'#13#3'not run'#13#3'DosError = %d';
                msg_programfileexitedwithexitcode = #3'Program %s'#13#3'exited with '#13#3'exitcode = %d';

                {Target platform dialog.}
                dialog_target = 'Target';
                label_target_platform = 'Target platform';

                {Primary file dialog.}
                label_primaryfile_primaryfile = 'Primary file';

                {Switches mode.}
                dialog_switchesmode = 'SwitchesMode';
                static_switchesmode_switchesmode = 'Switches Mode';

                {Compiler options.}
                dialog_compilerswitches = 'Compiler Switches';
                label_compiler_syntaxswitches = 'S~y~ntax Switches';
                label_compiler_mode = 'Compiler ~m~ode';
                label_compiler_codegeneration = 'Code generation';
                label_compiler_optimizations = 'Optimizations';
                label_compiler_opt_targetprocessor = 'Optimization target processor';
                label_compiler_codegen_targetprocessor = 'Code generation target processor';
                label_compiler_linkafter = 'Linking stage';
                label_compiler_verboseswitches = 'Verbose Switches';
                label_compiler_browser = 'Browser';
                label_compiler_assemblerreader = 'Assembler reader';
                label_compiler_assemblerinfo = 'Assembler info';
                label_compiler_assembleroutput = 'Assembler output';
                page_compiler_syntax = 'S~y~ntax';
                page_compiler_codegeneration = '~G~enerated code';
                page_compiler_verbose = '~V~erbose';
                page_compiler_browser = '~B~rowser';
                page_compiler_assembler = '~A~ssembler';

                {Memory sizes dialog.}
                dialog_memorysizes = 'Memory sizes';

                {Linker options dialog.}
                dialog_linker = 'Linker';
                label_linker_preferredlibtype = 'Preferred library type';

                {Debugger options dialog.}
                dialog_debugger = 'Browsing/Debugging/Profiling';
                label_debugger_debuginfo = 'Debugging information';
                label_debugger_profileswitches = 'Profiling Switches';
                label_debugger_compilerargs = 'Additional ~c~ompiler args';
                label_debugger_useanotherconsole = '~U~se another console';
                label_debugger_redirection = 'Debuggee ~R~edirection';
                label_debugger_useanothertty = '~U~se Another tty for Debuggee';

                { Remote options dialog }
                dialog_remote = 'Remote setup';
                label_remote_machine = 'Remote machine ~n~ame';
                label_remote_port = 'Remote ~p~ort number';
                label_remote_dir = 'Remote ~d~irectory';
                label_remote_config = 'Remote config ~o~ptions';
                label_remote_ident = 'Remote ~i~dent';
                label_remote_command = 'Remote ~c~ommand';
                label_remote_scp = 'Scp executable';
                label_remote_ssh = 'Ssh executable';

                {Directories dialog.}
                dialog_directories = 'Directories';

                {Editor options window.}
                dialog_defaulteditoroptions = 'Default Editor Options';
                dialog_editoroptions = 'Editor Options';
                label_editor_backupfiles = 'Create backup ~f~iles';
                label_editor_insertmode = '~I~nsert mode';
                label_editor_autoindentmode = '~A~uto indent mode';
                label_editor_usetabcharacters = '~U~se tab characters';
                label_editor_backspaceunindents = '~B~ackspace unindents';
                label_editor_persistentblocks = '~P~ersistent blocks';
                label_editor_syntaxhighlight = '~S~yntax highlight';
                label_editor_blockinsertcursor = 'B~l~ock insert cursor';
                label_editor_verticalblocks = '~V~ertical blocks';
                label_editor_highlightcolumn = 'Highlight ~c~olumn';
                label_editor_highlightrow = 'Highlight ~r~ow';
                label_editor_autoclosingbrackets = 'Aut~o~-closing brackets';
                label_editor_keeptrailingspaces = '~K~eep trailing spaces';
                label_editor_codecomplete = 'Co~d~eComplete enabled';
                label_editor_folds = 'E~n~able folds';
                label_editor_editoroptions = '~E~ditor options';
                label_editor_tabsize = '~T~ab size';
                label_editor_indentsize = 'Indent si~z~e';
                label_editor_highlightextensions = '~H~ighlight extensions';
                label_editor_filepatternsneedingtabs = 'File ~p~atterns needing tabs';

                {Browser options dialog.}
                dialog_browseroptions = 'Browser Options';
                dialog_localbrowseroptions = 'Local Browser Options';
                label_browser_labels = '~L~abels';
                label_browser_constants = '~C~onstants';
                label_browser_types = '~T~ypes';
                label_browser_variables = '~V~ariables';
                label_browser_procedures = '~P~rocedures';
                label_browser_inherited = '~I~nherited';
                label_browser_symbols = 'Symbols';
                label_browser_newbrowser = '~N~ew browser';
                label_browser_currentbrowser = '~R~eplace current';
                label_browser_subbrowsing = 'Sub-browsing';
                label_browser_scope = '~S~cope';
                label_browser_reference = 'R~e~ference';
                label_browser_preferredpane = 'Preferred pane';
                label_browser_qualifiedsymbols = '~Q~ualified symbols';
                label_browser_sortsymbols = 'S~o~rt always';
                label_browser_display = 'Display';

                {Preferences dialog.}
                dialog_preferences = 'Preferences';
                label_preferences_videomode = 'Video mode';
                label_preferences_currentdirectory = 'C~u~rrent directory';
                label_preferences_configdirectory = 'Conf~i~g file directory';
                label_preferences_desktopfile = 'Desktop file';
                label_preferences_editorfiles = 'Editor ~f~iles';
                label_preferences_environment = '~E~nvironment';
                label_preferences_desktop = '~D~esktop';
                label_preferences_autosave = 'Auto save';
                label_preferences_autotracksource = '~A~uto track source';
                label_preferences_closeongotosource = 'C~l~ose on go to source';
                label_preferences_changedironopen = 'C~h~ange dir on open';
                label_preferences_options = 'Options';

                {Desktop preferences dialog.}
                dialog_desktoppreferences = 'Desktop Preferences';
                label_desktop_historylists = '~H~istory lists';
                label_desktop_clipboard = '~C~lipboard content';
                label_desktop_watches = '~W~atch expressions';
                label_desktop_breakpoints = '~B~reakpoints';
                label_desktop_openwindow = '~O~pen windows';
                label_desktop_symbolinfo = '~S~ymbol information';
                label_desktop_codecompletewords = 'Co~d~eComplete wordlist';
                label_desktop_codetemplates = 'Code~T~emplates';
                label_desktop_preservedacrosssessions = '~P~reserved across sessions';

                {Mouse options dialog.}
                dialog_mouseoptions = 'Mouse Options';
                label_mouse_speedbar = 'Fast       Medium      Slow';
                label_mouse_doubleclickspeed = 'Mouse ~d~ouble click';
                label_mouse_reversebuttons = '~R~everse mouse buttons';
                label_mouse_crtlrightmousebuttonaction = 'Ctrl+Right mouse button';
                label_mouse_altrightmousebuttonaction = 'Alt+Right mouse button';
                label_mouse_act_nothing = 'Nothing';
                label_mouse_act_topicsearch = 'Topic search';
                label_mouse_act_gotocursor = 'Go to cursor';
                label_mouse_act_breakpoint = 'Breakpoint';
                label_mouse_act_evaluate = 'Evaluate';
                label_mouse_act_addwatch = 'Add watch';
                label_mouse_act_browsesymbol = 'Browse symbol';

                {Open options dialog.}
                dialog_openoptions = 'Open Options';
                msg_cantopenconfigfile = 'Can''t open config file.';
                msg_errorsavingconfigfile = 'Error saving config file.';

                {Save options dialog.}
                dialog_saveoptions = 'Save Options';
                dialog_ini_filename = 'Name of INI file';

                {Window list dialog.}
                dialog_windowlist = 'Window List';
                label_wndlist_windows = '~W~indows';
                msg_windowlist_hidden = 'hidden';

                {Help files dialog.}
                dialog_helpfiles = 'Install Help Files';
                label_helpfiles_helpfiles = '~H~elp files';

                {Install help file.}
                dialog_installhelpfile = 'Install a help file';
                label_installhelpfile_filename = '~H~elp file name';

                {Topic title dialog.}
                dialog_topictitle = 'Topic title';
                label_topictitle_title = 'Title';

                { About window }
{                dialog_about = 'About';
                label_about_compilerversion = 'Compiler Version';
                label_about_debugger = 'Debugger';}

                msg_errorparsingtoolparams = 'Error parsing tool params.';
                msg_executingtool = 'Executing tool %s ...';
                msg_errorreadingoutput = 'Error reading output.';
                msg_executingfilterfor = 'Executing filter for %s ...';
                msg_cantfindfilteredoutput = 'Can''t find filtered output.';
                msg_errorprocessingfilteredoutput = 'Error processing filtered output.';
                msg_errorexecutingfilter = 'Error executing filter %s';
                msg_errorexecutingtool = 'Error executing tool %s';
                msg_filterexecutionsuccessfulexitcodeis = 'Filter execution successful. Exit code = %d';
                msg_toolexecutionsuccessfulexitcodeis = 'Tool execution successful. Exit code = %d';
                msg_xmustbesettoyforz_doyouwanttochangethis =
                  '%s must be set to "%s" for %s. '+
                  'Do you want to change this option automatically?';

                dialog_greparguments = 'Grep arguments';
                msg_grepprogramnotfound = 'Grep program not found';
                label_grep_texttofind = '~T~ext to find';
                label_grep_greparguments = '~G~rep arguments';
                msg_runninggrepwithargs = 'Running Grep -n %s';
                msg_errorrunninggrep = #3'Error running Grep'#13#3'DosError = %d'#13#3'Exit code = %d';
                msg_errorreadinggrepoutput = #3'Error reading Grep output'#13#3'In line %d of %s';
                msg_filealreadyexistsoverwrite = 'File %s already exists. Overwrite?';
                msg_createkeywordindexforhelpfile = 'Create keyword index from help file?';

                msg_pleasewaitwhilecreatingindex = 'Please wait while creating index...';
                msg_buildingindexfile = 'Building index file %s';
                msg_filedoesnotcontainanylinks = '%s doesn''t contain any links, thus it isn''t suitable for indexing.';
                msg_storinghtmlindexinfile = 'Storing HTML index in %s';
                msg_errorstoringindexdata = 'Error storing index data (%d)';

                msg_cantcreatefile = 'Can''t create %s';

                {ANSI screenshots.}
                msg_saveansifile = 'Save previous screen as Ansi File';
                msg_click_upper_left = 'Click to select upper left corner; Escape to cancel; Enter to select (0,0)';
                msg_click_lower_right = 'Click to select lower right corner; Escape to cancel; Enter to select (maxX,maxY)';
