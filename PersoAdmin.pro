QT += core gui sql charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Database/database_controller.cpp \
    Database/postgres_controller.cpp \
    Database/database_table_model.cpp \
    GUI/gui_authorization.cpp \
    GUI/gui_master.cpp \
    GUI/main_window_kernel.cpp \
    GUI/gui.cpp \
    GUI/gui_delegates.cpp \
    GUI/user_interaction_system.cpp \
    Management/admin_manager.cpp \
    Log/log_system.cpp \
    Management/administration_system.cpp \
    Management/administration_system_builder.cpp \
    main.cpp

HEADERS += \
    GUI/gui_authorization.h \
    Management/admin_manager.h \
    General/definitions.h \
    Database/database_controller.h \
    Database/postgres_controller.h \
    Database/database_table_model.h \
    GUI/gui_master.h \
    GUI/main_window_kernel.h \
    GUI/gui.h \
    GUI/gui_delegates.h \
    GUI/user_interaction_system.h \
    Log/log_system.h \
    Management/administration_system.h \
    Management/administration_system_builder.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
