require 'mpq_read'

module MPQ
  class Archive
    attr_reader :path, :files
    LISTFILE = "(listfile)"

    def initialize(path)
      raise LoadError, "#{path.inspect} not found" unless File.exists? path
      @path = path
    end

    def files
      @files ||= read_file(LISTFILE).split("\r\n")
    end
  end
end
