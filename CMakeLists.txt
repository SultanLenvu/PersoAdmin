cmake_minimum_required(VERSION 3.16)
project(PersoAdmin VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#========================================== INCLUSION OF QT ===========================================#
find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts Gui Network PrintSupport Sql Core5Compat)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_standard_project_setup()
#======================================================================================================#

#========================================== SOURCE DIRECTORIES ===========================================#
include_directories(Database/)
include_directories(General/)
include_directories(Functors/)
include_directories(GUI/)
include_directories(GUI/Subkernels)
include_directories(GUI/Dialogs)
include_directories(GUI/Widgets)
include_directories(Log/)
include_directories(Management/)
include_directories(StickerPrinter/)
include_directories(ServerConnection/)
include_directories(ServerConnection/Commands)
#======================================================================================================#

add_executable(PersoAdmin WIN32
    Database/abstract_sql_database.cpp Database/abstract_sql_database.h
    Database/postgre_sql_database.cpp Database/postgre_sql_database.h
    Database/postgre_sql_table.cpp Database/postgre_sql_table.h
    Database/sql_table_relation.cpp Database/sql_table_relation.h
    Database/abstract_sql_table.cpp Database/abstract_sql_table.h
    Database/sql_query_values.h Database/sql_query_values.cpp
    Database/sql_insert_values.h Database/sql_insert_values.cpp


    GUI/authorization_gui.cpp GUI/authorization_gui.h
    GUI/interaction_system.cpp GUI/interaction_system.h
    GUI/mainwindow_gui.cpp GUI/mainwindow_gui.h
    GUI/gui_kernel.cpp GUI/gui_kernel.h

    GUI/Subkernels/abstract_gui_subkernel.h GUI/Subkernels/abstract_gui_subkernel.cpp
    GUI/Subkernels/order_gui_subkernel.h GUI/Subkernels/order_gui_subkernel.cpp
    GUI/Subkernels/perso_server_gui_subkernel.h GUI/Subkernels/perso_server_gui_subkernel.cpp
    GUI/Subkernels/production_line_gui_subkernel.h GUI/Subkernels/production_line_gui_subkernel.cpp
    GUI/Subkernels/programmer_gui_subkernel.h GUI/Subkernels/programmer_gui_subkernel.cpp
    GUI/Subkernels/sticker_printer_gui_subkernel.h GUI/Subkernels/sticker_printer_gui_subkernel.cpp
    GUI/Subkernels/database_gui_subkernel.h GUI/Subkernels/database_gui_subkernel.cpp

    GUI/Dialogs/abstract_input_dialog.cpp GUI/Dialogs/abstract_input_dialog.h
    GUI/Dialogs/pallet_shiping_dialog.cpp GUI/Dialogs/pallet_shiping_dialog.h
    GUI/Dialogs/pan_input_dialog.cpp GUI/Dialogs/pan_input_dialog.h
    GUI/Dialogs/string_input_dialog.h GUI/Dialogs/string_input_dialog.cpp
    GUI/Dialogs/production_line_creation_dialog.h GUI/Dialogs/production_line_creation_dialog.cpp
    GUI/Dialogs/order_creation_dialog.h GUI/Dialogs/order_creation_dialog.cpp
    GUI/Dialogs/manual_release_refund_dialog.h GUI/Dialogs/manual_release_refund_dialog.cpp
    GUI/Dialogs/link_issuer_key_dialog.h GUI/Dialogs/link_issuer_key_dialog.cpp
    GUI/Dialogs/settings_dialog.h GUI/Dialogs/settings_dialog.cpp
    GUI/Dialogs/authorization_dialog.cpp GUI/Dialogs/authorization_dialog.h
    GUI/Dialogs/transponder_sticker_scan_dialog.cpp GUI/Dialogs/transponder_sticker_scan_dialog.h

    GUI/Widgets/order_user_interface.h GUI/Widgets/order_user_interface.cpp
    GUI/Widgets/database_user_interface.h GUI/Widgets/database_user_interface.cpp
    GUI/Widgets/production_line_user_interface.h GUI/Widgets/production_line_user_interface.cpp
    GUI/Widgets/sticker_printer_user_interface.h GUI/Widgets/sticker_printer_user_interface.cpp
    GUI/Widgets/perso_server_user_interface.h GUI/Widgets/perso_server_user_interface.cpp

    General/types.h
    General/definitions.h
    General/sql_response_model.cpp General/sql_response_model.h
    General/pobject.cpp General/pobject.h
    General/hash_table_model.cpp General/hash_table_model.h
    General/configurable_object.h General/configurable_object.cpp

    Functors/abstract_string_checker.h Functors/abstract_string_checker.cpp
    Functors/ucid_checker.h Functors/ucid_checker.cpp
    Functors/numeric_string_checker.h Functors/numeric_string_checker.cpp

    Log/file_log_backend.cpp Log/file_log_backend.h
    Log/log_backend.cpp Log/log_backend.h
    Log/log_system.cpp Log/log_system.h
    Log/widget_log_backend.cpp Log/widget_log_backend.h

    Management/abstract_manager.h Management/abstract_manager.cpp

    Management/global_environment.cpp Management/global_environment.h
    Management/perso_server_manager.cpp Management/perso_server_manager.h
    Management/database_manager.h Management/database_manager.cpp
    Management/order_manager.h Management/order_manager.cpp
    Management/production_line_manager.h Management/production_line_manager.cpp
    Management/programmer_manager.h Management/programmer_manager.cpp
    Management/sticker_printer_manager.h Management/sticker_printer_manager.cpp

    ServerConnection/abstract_server_connection.cpp ServerConnection/abstract_server_connection.h
    ServerConnection/perso_server_connection.cpp ServerConnection/perso_server_connection.h
    ServerConnection/Commands/abstract_client_command.cpp ServerConnection/Commands/abstract_client_command.h
    ServerConnection/Commands/complete_current_box.cpp ServerConnection/Commands/complete_current_box.h
    ServerConnection/Commands/confirm_transponder_release.cpp ServerConnection/Commands/confirm_transponder_release.h
    ServerConnection/Commands/confirm_transponder_rerelease.cpp ServerConnection/Commands/confirm_transponder_rerelease.h
    ServerConnection/Commands/echo.cpp ServerConnection/Commands/echo.h ServerConnection/Commands/get_current_box_data.cpp
    ServerConnection/Commands/get_current_box_data.h ServerConnection/Commands/get_current_transponder_data.cpp
    ServerConnection/Commands/get_current_transponder_data.h ServerConnection/Commands/get_production_line_data.cpp
    ServerConnection/Commands/get_production_line_data.h ServerConnection/Commands/get_transponder_data.cpp
    ServerConnection/Commands/get_transponder_data.h ServerConnection/Commands/launch_production_line.cpp
    ServerConnection/Commands/launch_production_line.h ServerConnection/Commands/print_box_sticker.cpp
    ServerConnection/Commands/print_box_sticker.h ServerConnection/Commands/print_last_box_sticker.cpp
    ServerConnection/Commands/print_last_box_sticker.h ServerConnection/Commands/print_last_pallet_sticker.cpp
    ServerConnection/Commands/print_last_pallet_sticker.h ServerConnection/Commands/print_pallet_sticker.cpp
    ServerConnection/Commands/print_pallet_sticker.h ServerConnection/Commands/refund_current_box.cpp
    ServerConnection/Commands/refund_current_box.h ServerConnection/Commands/release_transponder.cpp
    ServerConnection/Commands/release_transponder.h ServerConnection/Commands/request_box.cpp
    ServerConnection/Commands/request_box.h ServerConnection/Commands/rerelease_transponder.cpp
    ServerConnection/Commands/rerelease_transponder.h ServerConnection/Commands/rollback_transponder.cpp
    ServerConnection/Commands/rollback_transponder.h ServerConnection/Commands/shutdown_production_line.cpp
    ServerConnection/Commands/shutdown_production_line.h

    StickerPrinter/abstract_sticker_printer.cpp StickerPrinter/abstract_sticker_printer.h
    StickerPrinter/te310_printer.cpp StickerPrinter/te310_printer.h

    main.cpp
    GUI/Subkernels/abstract_manager_signal_handler.h GUI/Subkernels/abstract_manager_signal_handler.cpp
    )

target_compile_definitions(PersoAdmin PRIVATE
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

target_link_libraries(PersoAdmin PRIVATE
    Qt::Charts
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::PrintSupport
    Qt::Sql
    Qt::Core5Compat
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(PersoAdmin PRIVATE
        Qt::Widgets
    )
endif()

install(TARGETS PersoAdmin
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET PersoAdmin
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
