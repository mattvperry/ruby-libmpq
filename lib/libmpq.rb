require 'mpq_read'

class MPQ::Archive
  attr_reader :path
  LISTFILE = "(listfile)"

  def initialize(path)
    @path = path
  end

  def list_files
    read_file(LISTFILE).split("\r\n")
  end
end
