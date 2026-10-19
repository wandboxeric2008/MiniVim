# minivim汇总
## 一、类
### 1.`Position`
位置：Types.hpp

含义：光标在buffer中的位置
- `int row_`：光标在buffer中的行号
- `int column_`：光标在buffer中的字符下标

### 2.`ScreenSize`
位置：Types.hpp

含义：终端的尺寸

备注：终端=窗口+命令提示行

- `int rows_`：终端的总行数
- `int columns_`：终端的总显示列数

### 3.`Viewport`
位置：Types.hpp

含义：窗口可见的buffer范围

- `int top_`：窗口最顶部可见的buffer行号
- `int left_`：窗口最左侧可见的buffer显示列号
- `int rows_`：窗口可见的buffer行数
- `int columns_`：窗口可见的显示列数

### 4.`KeyEvent`
位置：Key.hpp

含义：按键信息

- `KeyCode code_`：按键类型
- `char value_`：按键内容

## 二、枚举类

### 1.`Mode`
位置：Types.hpp

含义：当前的编辑模式

- `Normal`：普通模式
- `Insert`：插入模式
- `CommandLine`：冒号命令行模式

### 2.`Motion`
位置：Types.hpp

含义：光标移动一格的方向

- `Left`
- `Down`
- `Up`
- `Right`

### 3.`KeyCode`
位置：Key.hpp

含义：按键类型

- `Character`：字母或ctrl+字母
- `Escape`
- `Enter`
- `Backspace`
- `Delete`

## 三、函数

### 1.`char ControlKey(char key)`
位置：Key.hpp

功能：输入按键，返回ctrl+按键

### 2.`KeyEvent Character(char value)`
位置：Key.hpp

功能：输入字母按键，返回字母按键信息

### 3.`bool IsCharacter(char expected)`
位置：Key.hpp

功能：输入按键，判断当前KeyEvent是否为此按键

### 4.`bool IsControl(char expected)`
位置：Key.hpp

功能：输入按键，判断当前KeyEvent是否为ctrl+此按键

# `位置：TextLayout.hpp`

## 职能：字符下标和显示列之间的换算函数

### 一、变量

#### 1.`tabStop=4`

### 二、函数

#### 1.`int NextScreenColumn(int column,char value)`：输入当前字符和显示列，返回下一个显示列

#### 2.`int RenderColumnToBufferColumn(string_view line,int render_column)`：把显示列转换为字符下标，若显示列超出行尾则返回最右字符下标

#### 3.`BufferColumnToRenderColumn(string_view line,int buffer_column)`：把字符下标转换为显示列，若字符下标超出行尾则返回最右显示列

# `Buffer.cpp\.hpp`

## 职能：缓冲区相关功能函数

### 一、函数

#### 1.`Buffer(filesystem::path& path)`：从输入的path指向的文件新建buffer

#### 2.`Buffer(vector<string> lines,filesystem::path path)`：

#### 3.`int GetLineCount()`：返回文件行数

#### 4.`string GetLineAt(int row)`：返回文件第row行

#### 5.`string GetDisplayName()`：返回文件名，若是新文件，返回[No Name]

#### 6.`bool IsModified()`：判断返回文件和上次保存比起来是否被修改过

#### 7.`void InsertCharacter(int row,int column,char value)`：在第row行第column列插入一个字符

#### 8.`void EraseCharacter(int row,int column)`：在第row行第column列删除一个字符

#### 9.`void SplitLine(int row,int column)`：在第row行第column列输入回车

#### 10.`void BJoinLine(int row)`：把第row+1行合并进第row行

#### 11.`void Save()`：保存文件内容（调用WriteTo）

#### 12.`void SaveAs(filesystem::path& path)`：另存为

#### 13.`void WriteTo(filesystem::path& path)`：将buffer中内容写入文件

# `Window.cpp\.hpp`

## 职能：终端相关功能函数

### 一、函数

#### 1.`void Resize(ScreenSize terminal_size)`：根据输入的终端大小输出可视范围，正文（不含命令提示行）的行、列数均≥1

#### 2.`void ApplyMotion(Buffer& buffer,Motion motion)`：调整视口，让移动后的光标可见

#### 3.`void EnsureCursorVisible(Buffer& buffer)`：调整视口，让移动后的光标可见

#### 4.`Position& GetCursor()`：返回光标位置

#### 5.`Viewport& GetViewport()`：返回终端可视范围

#### 6.`void SetCursor(Buffer& buffer,Position position,bool allow_line_end)`：放置光标

#### 7.`void MoveLeft(Buffer& buffer,int count)`：左移count个字符,并更新目标显示列

#### 8.`void MoveRight(Buffer& buffer,int count)`：右移count个字符,并更新目标显示列

#### 9.`void MoveUp(Buffer& buffer,int count)`：上移

#### 10.`void MoveDown(Buffer& buffer,int count)`：下移

# `Command.cpp\.hpp`

## 职能

### 一、类

#### 1.`EditorAction`：需要Editor操作的信息

- `ActionKind kind_`：命令类型信息
- `optional<Motion> motion_`：移动信息

#### 2.`NormalModeParser`：

- `EditorAction Feed(KeyEvent key)`：
- `EditorAction GenerateMotion(Motion motion)`：
- `EditorAction GenerateCommand(ActionKind kind)`：


### 二、枚举类

#### 1.`ActionKind`：输入的命令类型

- `None`：占位符
- `Move`：移动
- `InsertBefore,InsertAfter`：i
- `EnterCommandLine`：a

# `Renderer.cpp\.hpp`

## 职能

### 类

#### 1.`RenderState`

- `Mode mode_`：
- `string command_`：
- `string message_`：

#### 2.`Renderer`

- `string Render(Buffer& buffer,Window& window,RenderState& state)`：
- `string ExpandForDisplay(string_view line)`：

# `Editor.cpp\.hpp`

## 职能

### 一、类

#### 1.`Editor`：

- `Editor(filesystem::path& path={})`：
- `void Run()`：
- `bool IsRunning()`：
- `void RefreshScreen()`：
- `void ProcessKey(KeyEvent key)`：
- `void Execute(EditorAction& action)`：
- `void EnterInsert(Position position)`：
- `void LeaveInsert()`：
- `void HandleInsert(KeyEvent key)`：
- `void HandleCommandLine(KeyEvent key)`：
- `void ExecuteCommandLine()`：
- `void LeaveCommandLine()`：
- `bool SaveBuffer(filesystem::path& path={})`：
- `Buffer buffer_`：
- `Terminal terminal_`：
- `Window window_`：
- `Renderer renderer_`：
- `NormalModeParser normal_parser_`：
- `Mode mode_`：
- `string command_`：
- `string message_`：
- `bool running_`：

# `Terminal.cpp\.hpp`

## 职能

### 一、类

#### `Terminal`