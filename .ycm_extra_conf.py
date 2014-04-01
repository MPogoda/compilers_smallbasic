flags = [
'-Wall',
'-Wextra',
'-Werror',
'-Wno-long-long',
'-Wno-variadic-macros',
'-fexceptions',
'-I/usr/include/',
'-std=c++11',
'-x',
'c++',
'-DQT_CORE_LIB',
'-DQT_DEBUG',
'-DQT_GUI_LIB',
'-isystem',
'/usr/include/qt4',
'-isystem',
'/usr/include/qt4/QtGui',
'-isystem',
'/usr/include/qt4/QtCore'
]

def FlagsForFile(filename, **kwargs):
    return {'flags': flags, 'do_cache': True}
