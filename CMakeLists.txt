cmake_minimum_required(VERSION 3.16)
project(PersoAdmin VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts Gui Network PrintSupport Sql Core5Compat)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_standard_project_setup()

add_executable(PersoAdmin WIN32 MACOSX_BUNDLE
    Database/abstract_sql_database.cpp Database/abstract_sql_database.h
    Database/postgre_sql_database.cpp Database/postgre_sql_database.h
    Database/postgre_sql_table.cpp Database/postgre_sql_table.h
    Database/sql_table_relation.cpp Database/sql_table_relation.h
    Database/abstract_sql_table.cpp Database/abstract_sql_table.h
    Database/sql_query_values.h Database/sql_query_values.cpp

    GUI/abstract_gui.cpp GUI/abstract_gui.h
    GUI/authorization_gui.cpp GUI/authorization_gui.h
    GUI/interaction_system.cpp GUI/interaction_system.h
    GUI/mainwindow_gui.cpp GUI/mainwindow_gui.h
    GUI/mainwindow_kernel.cpp GUI/mainwindow_kernel.h

    GUI/Dialogs/abstract_input_dialog.cpp GUI/Dialogs/abstract_input_dialog.h
    GUI/Dialogs/pallet_shiping_dialog.cpp GUI/Dialogs/pallet_shiping_dialog.h
    GUI/Dialogs/pan_input_dialog.cpp GUI/Dialogs/pan_input_dialog.h
    GUI/Dialogs/idetifier_input_dialog.h GUI/Dialogs/identifier_input_dialog.cpp
    GUI/Dialogs/production_line_creation_dialog.h GUI/Dialogs/production_line_creation_dialog.cpp
    GUI/Dialogs/order_creation_dialog.h GUI/Dialogs/order_creation_dialog.cpp
    GUI/Dialogs/start_production_line_dialog.h GUI/Dialogs/start_production_line_dialog.cpp
    GUI/Dialogs/manual_release_refund_dialog.h GUI/Dialogs/manual_release_refund_dialog.cpp
    GUI/Dialogs/link_issuer_key_dialog.h GUI/Dialogs/link_issuer_key_dialog.cpp
    GUI/Dialogs/settings_dialog.h GUI/Dialogs/settings_dialog.cpp

    General/definitions.h
    General/hash_model.cpp General/hash_model.h

    Log/file_log_backend.cpp Log/file_log_backend.h
    Log/log_backend.cpp Log/log_backend.h
    Log/log_system.cpp Log/log_system.h
    Log/widget_log_backend.cpp Log/widget_log_backend.h

    Management/admin_manager.cpp Management/admin_manager.h
    Management/administration_system.cpp Management/administration_system.h
    Management/perso_client.cpp Management/perso_client.h

    StickerPrinter/isticker_printer.cpp StickerPrinter/isticker_printer.h
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
