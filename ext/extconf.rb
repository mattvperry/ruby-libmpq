require 'mkmf'

name = 'mpq_read'
have_library('mpq')
dir_config(name)
create_makefile(name)
