QT += core gui sql charts network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Database/database_controller.cpp \
    Database/postgres_controller.cpp \
    Database/database_table_model.cpp \
    GUI/custom_input_dialog.cpp \
    GUI/gui_authorization.cpp \
    GUI/gui_master.cpp \
    GUI/gui.cpp \
    GUI/gui_delegates.cpp \
    GUI/interaction_system.cpp \
    GUI/main_window_kernel.cpp \
    GUI/pallet_shiping_dialog.cpp \
    GUI/pan_input_dialog.cpp \
    General/hash_model.cpp \
    Log/file_log_backend.cpp \
    Log/widget_log_backend.cpp \
    Log/log_backend.cpp \
    Log/log_system.cpp \
    Management/admin_manager.cpp \
    Management/administration_system.cpp \
    Management/perso_client.cpp \
    StickerPrinter/isticker_printer.cpp \
    StickerPrinter/te310_printer.cpp \
    main.cpp

HEADERS += \
    GUI/custom_input_dialog.h \
    GUI/interaction_system.h \
    GUI/main_window_kernel.h \
    GUI/pallet_shiping_dialog.h \
    GUI/pan_input_dialog.h \
    General/hash_model.h \
    Log/file_log_backend.h \
    Log/widget_log_backend.h \
    Log/log_backend.h \
    Log/log_system.h \
    General/definitions.h \
    Database/database_controller.h \
    Database/postgres_controller.h \
    Database/database_table_model.h \
    GUI/gui_authorization.h \
    GUI/gui_master.h \
    GUI/gui.h \
    GUI/gui_delegates.h \
    Management/admin_manager.h \
    Management/administration_system.h \
    Management/perso_client.h \
    StickerPrinter/isticker_printer.h \
    StickerPrinter/te310_printer.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
