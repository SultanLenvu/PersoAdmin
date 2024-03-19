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
include_directories(Service/)
include_directories(Service/General/)
include_directories(Service/Log/)
include_directories(Service/Functors/)
include_directories(Service/Reactions/)
include_directories(Service/ObjectTypes/)
include_directories(Database/)
include_directories(GUI/)
include_directories(GUI/Subkernels)
include_directories(GUI/Dialogs)
include_directories(GUI/Widgets)
include_directories(GUI/Reactions)
include_directories(Management/Managers)
include_directories(Management/AsyncWrappers)
include_directories(StickerPrinter/)
include_directories(ServerConnection/)
include_directories(ServerConnection/Commands)
#======================================================================================================#

add_executable(PersoAdmin WIN32
    Service/General/types.h
    Service/General/definitions.h
    Service/General/sql_response_model.cpp Service/General/sql_response_model.h
    Service/General/hash_table_model.cpp Service/General/hash_table_model.h

    Service/General/global_environment.cpp Service/General/global_environment.h

    Service/General/named_object_factory.h Service/General/named_object_factory.cpp

    Service/ObjectTypes/named_object.cpp Service/ObjectTypes/named_object.h
    Service/ObjectTypes/configurable_object.h Service/ObjectTypes/configurable_object.cpp
    Service/ObjectTypes/loggable_object.h Service/ObjectTypes/loggable_object.cpp
    Service/ObjectTypes/log_system_connection.h Service/ObjectTypes/log_system_connection.cpp

    Service/i_service_object_space.h Service/i_service_object_space.cpp
    Service/service_object_space.h Service/service_object_space.cpp
    Service/asynchronous_object_space.h Service/asynchronous_object_space.cpp
    Service/configuration_manager.h Service/configuration_manager.cpp

    Service/Functors/abstract_string_checker.h Service/Functors/abstract_string_checker.cpp
    Service/Functors/ucid_checker.h Service/Functors/ucid_checker.cpp
    Service/Functors/numeric_string_checker.h Service/Functors/numeric_string_checker.cpp

    Service/Log/file_log_backend.cpp Service/Log/file_log_backend.h
    Service/Log/abstract_log_backend.cpp Service/Log/abstract_log_backend.h
    Service/Log/log_system.cpp Service/Log/log_system.h
    Service/Log/widget_log_backend.cpp Service/Log/widget_log_backend.h

    Database/i_sql_database.cpp Database/i_sql_database.h
    Database/postgre_sql_database.cpp Database/postgre_sql_database.h
    Database/postgre_sql_table.cpp Database/postgre_sql_table.h
    Database/sql_table_relation.cpp Database/sql_table_relation.h
    Database/abstract_sql_table.cpp Database/abstract_sql_table.h
    Database/sql_query_values.h Database/sql_query_values.cpp
    Database/sql_insert_values.h Database/sql_insert_values.cpp

    GUI/authorization_gui.cpp GUI/authorization_gui.h
    GUI/mainwindow_gui.cpp GUI/mainwindow_gui.h
    GUI/gui_kernel.cpp GUI/gui_kernel.h

    GUI/Subkernels/response_ready_connection.h GUI/Subkernels/response_ready_connection.cpp
    GUI/Subkernels/abstract_gui_subkernel.h GUI/Subkernels/abstract_gui_subkernel.cpp
    GUI/Subkernels/order_gui_subkernel.h GUI/Subkernels/order_gui_subkernel.cpp
    GUI/Subkernels/server_gui_subkernel.h GUI/Subkernels/server_gui_subkernel.cpp
    GUI/Subkernels/production_line_gui_subkernel.h GUI/Subkernels/production_line_gui_subkernel.cpp
    GUI/Subkernels/programmer_gui_subkernel.h GUI/Subkernels/programmer_gui_subkernel.cpp
    GUI/Subkernels/sticker_printer_gui_subkernel.h GUI/Subkernels/sticker_printer_gui_subkernel.cpp
    GUI/Subkernels/database_gui_subkernel.h GUI/Subkernels/database_gui_subkernel.cpp
    GUI/Subkernels/assembly_unit_gui_subkernel.h GUI/Subkernels/assembly_unit_gui_subkernel.cpp

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

    GUI/Widgets/abstract_user_interface.h GUI/Widgets/abstract_user_interface.cpp
    GUI/Widgets/order_user_interface.h GUI/Widgets/order_user_interface.cpp
    GUI/Widgets/database_user_interface.h GUI/Widgets/database_user_interface.cpp
    GUI/Widgets/production_line_user_interface.h GUI/Widgets/production_line_user_interface.cpp
    GUI/Widgets/sticker_printer_user_interface.h GUI/Widgets/sticker_printer_user_interface.cpp
    GUI/Widgets/server_user_interface.h GUI/Widgets/server_user_interface.cpp

    GUI/Reactions/i_progress_indicator.h GUI/Reactions/i_progress_indicator.cpp
    GUI/Reactions/progress_indicator.cpp GUI/Reactions/progress_indicator.h
    GUI/Reactions/progress_indicator_link.h GUI/Reactions/progress_indicator_link.cpp
    GUI/Reactions/i_status_indicator.h GUI/Reactions/i_status_indicator.cpp
    GUI/Reactions/status_indicator.h GUI/Reactions/status_indicator.cpp
    GUI/Reactions/status_indicator_link.h GUI/Reactions/status_indicator_link.cpp

    Management/Managers/i_manager.h Management/Managers/i_manager.cpp
    Management/Managers/order_manager.h Management/Managers/order_manager.cpp
    Management/Managers/production_line_manager.h Management/Managers/production_line_manager.cpp
    Management/Managers/assembly_unit_manager.h Management/Managers/assembly_unit_manager.cpp

    Management/AsyncWrappers/database_async_wrapper.h Management/AsyncWrappers/database_async_wrapper.cpp
    Management/AsyncWrappers/i_async_wrapper.h Management/AsyncWrappers/i_async_wrapper.cpp
    Management/AsyncWrappers/progressable_async_wrapper.h Management/AsyncWrappers/progressable_async_wrapper.cpp
    Management/AsyncWrappers/order_manager_async_wrapper.h Management/AsyncWrappers/order_manager_async_wrapper.cpp
    Management/AsyncWrappers/production_line_manager_async_wrapper.h Management/AsyncWrappers/production_line_manager_async_wrapper.cpp
    Management/AsyncWrappers/programmer_async_wrapper.h Management/AsyncWrappers/programmer_async_wrapper.cpp
    Management/AsyncWrappers/sticker_printer_async_wrapper.h Management/AsyncWrappers/sticker_printer_async_wrapper.cpp
    Management/AsyncWrappers/server_connection_async_wrapper.cpp Management/AsyncWrappers/server_connection_async_wrapper.h
    Management/AsyncWrappers/assembly_unit_manager_async_wrapper.h Management/AsyncWrappers/assembly_unit_manager_async_wrapper.cpp

    ServerConnection/i_server_connection.cpp ServerConnection/i_server_connection.h
    ServerConnection/perso_server_connection.cpp ServerConnection/perso_server_connection.h
    ServerConnection/server_disconnected_signal.h ServerConnection/server_disconnected_signal.cpp

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
    ServerConnection/Commands/shutdown_production_line.h ServerConnection/Commands/shutdown_production_line.cpp

    StickerPrinter/i_sticker_printer.cpp StickerPrinter/i_sticker_printer.h
    StickerPrinter/te310_printer.cpp StickerPrinter/te310_printer.h

    main.cpp
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
