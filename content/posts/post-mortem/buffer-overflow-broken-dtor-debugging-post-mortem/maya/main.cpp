#include <string>
#include <algorithm>

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <sys/mman.h>
#include <sys/stat.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

class ScriptEngine {
public:
  ScriptEngine(const char *filename);
  ~ScriptEngine();

  void parseLine(bool *has_next);
  void executeLine();

private:
  void open();
  void close();

private:
  // state
  int curr_pos_;
  char cmd_[8];
  char name_[8];
  char text_[8];

  // raw script content
  std::string filename_;
  const char *data_;
  int length_;
  int fd_;
};


int main(int argc, char *argv[0])
{
  if(argc != 2) {
    printf("Usage: %s <script filepath>\n", argv[0]);
    exit(0);
  }

  ScriptEngine script_engine(argv[1]);

  bool has_next = true;
  while(has_next) {
    script_engine.parseLine(&has_next);
    script_engine.executeLine();
  }
  return 0;
}

// implement ScriptEngine
ScriptEngine::ScriptEngine(const char *filename)
  : curr_pos_(0), filename_(filename), data_(nullptr), length_(-1), fd_(-1)
{
  std::fill(cmd_, cmd_ + sizeof(cmd_), 0);
  std::fill(name_, name_ + sizeof(name_), 0);
  std::fill(text_, text_ + sizeof(text_), 0);

  open();
}

ScriptEngine::~ScriptEngine()
{
  close();
}

void ScriptEngine::open()
{
  // open
  fd_ = ::open(filename_.data(), O_RDONLY, 0);
  if(fd_ == -1) {
    err(1, "open");
  }

  // get file size
  struct stat sb;
  if(fstat(fd_, &sb) < 0) {
    err(1, "fstat");
  }
  length_ = sb.st_size;

  //memory map
  data_ = (char *)mmap(NULL, length_, PROT_READ, MAP_SHARED, fd_, 0);
  if(data_ == MAP_FAILED) {
    err(1, "mmap");
  }
}

void ScriptEngine::close()
{
  if(fd_ != -1) {
    munmap((void*)data_, length_);
    ::close(fd_);

    fd_ = -1;
  }
}

void ScriptEngine::parseLine(bool *has_next)
{
  // syntax
  // <cmd> <name>:<text>
  enum ParseState {
    STATE_CMD,
    STATE_NAME,
    STATE_TEXT,
    STATE_FINISH,
  };

  struct ParseCommand {
    ParseState curr_state;
    ParseState next_state;
    char delim;
    char *buffer;
    int buffer_size;
  };

  ParseCommand cmds[] = {
    { STATE_CMD, STATE_NAME, ' ', cmd_, sizeof(cmd_) },
    { STATE_NAME, STATE_TEXT, ':', name_, sizeof(name_) },
    { STATE_TEXT, STATE_FINISH, '\n', text_, sizeof(text_) },
  };

  ParseState parse_state = STATE_CMD;

  char buffer[1024];
  int len = 0;

  while((curr_pos_ < length_) && (parse_state != STATE_FINISH)) {
    char ch = data_[curr_pos_++];

    for(const ParseCommand &cmd : cmds) {
      if(cmd.curr_state == parse_state) {
        if(cmd.delim == ch) {
          memset(cmd.buffer, 0, cmd.buffer_size);
          memcpy(cmd.buffer, buffer, len);

          parse_state = cmd.next_state;
          len = 0;
        } else {
          buffer[len++] = ch;
        }
        break;
      }
    }
  }

  *has_next = (curr_pos_ < length_);
}

void ScriptEngine::executeLine()
{
  // add complex feature here
  if(std::string("play") == cmd_) {
    printf("%s %s %s.\n", name_, cmd_, text_);

  } else if(std::string("say") == cmd_) {
    printf("%s %s \"%s\".\n", name_, cmd_, text_);

  } else {
    printf("Unknown: cmd=%s, name=%s, text=%s.\n", cmd_, name_, text_);
  }
}
