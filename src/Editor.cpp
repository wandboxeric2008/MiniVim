#include "Editor.hpp"

#include <algorithm>
#include <cctype>
#include <exception>

//以下提示均可用于Basic部分的实现,部分函数在Advanced部分中需要修改
//在该文件中,有些函数我们完整保留了正确的实现,有些函数去掉了一些,其余的完全需要你自己填写
namespace sjtu {

namespace {
//匿名namespace,提供了只给当前文件使用的辅助函数

std::string Trim(std::string value) {
    //去掉字符串两端的空白,保留中间的内容;全部是空白时返回空字符串
    //你可以分别从两端找到第一个非空白字符,注意反向迭代器转回正向迭代器时的边界
    unsigned long lengthofvalue{value.size()};
    if(lengthofvalue>=1){
        std::string cutleftvalue{""};
        char firstofvalue{value[0]};
        if(firstofvalue==' '||firstofvalue=='\t'){
            bool cut{true};
            for(int i=0;i+1<=lengthofvalue;i++){
                char iofvalue{value[i]};
                if(iofvalue!=' '&&iofvalue!='\t'){
                    cut=false;
                }
                if(cut==false){
                    cutleftvalue+=iofvalue;
                }
            }
        }
        else{
            cutleftvalue=value;
        }
        if(cutleftvalue.size()>=2){
            while(cutleftvalue.size()>=1&&(cutleftvalue[cutleftvalue.size()-1]==' '||cutleftvalue[cutleftvalue.size()-1]=='\t')){
                cutleftvalue.pop_back();
            }
        }
        return cutleftvalue;
    }
    else{
        return "";
    }
}

//判断是否为ASCII可打印字符,Tab由插入模式另外处理
bool IsPrintable(char value) { return value >= 0x20U && value < 0x7FU; }

} // namespace

//用path初始化文件缓冲区,Terminal构造时会准备好终端输入环境
Editor::Editor(const std::filesystem::path& path) : buffer_(path), terminal_() {}

void Editor::Run() {
    //每轮先刷新画面,再读取并处理一个按键;退出循环后清屏
    while (running_) {
        RefreshScreen();
        ProcessKey(terminal_.ReadKey());
    }
    terminal_.ClearScreen();
}

//返回编辑器是否还需要继续运行
bool Editor::IsRunning() const noexcept { return running_; }

void Editor::RefreshScreen() {
    //1. 获取终端大小(GetScreenSize),更新窗口可显示的范围,并让光标落在可见区域内
    //2. 把当前模式、命令和提示打包成RenderState
    //3. 让Renderer生成一帧字符串,再交给Terminal输出
    //这里是你唯一需要调用Terminal中的接口的地方,请调用WriteOutput
    //throw std::runtime_error("Not implemented.");
    window_.Resize(terminal_.GetScreenSize());
    window_.EnsureCursorVisible(buffer_);
    terminal_.WriteOutput(renderer_.Render(buffer_,window_,{mode_,command_,message_}));
}

void Editor::ProcessKey(KeyEvent key) {
    //1. (可选)先处理所有模式都能使用的Ctrl-Q,用于紧急退出
    //2. 按当前模式分发给命令行或插入模式的处理函数
    //3. Normal模式下清除旧提示,将按键交给parser,再执行生成的Action
    //throw std::runtime_error("Not implemented.");
    if(key.IsControl('q')){
        running_=false;
        return;
    }
    EditorAction action;
    switch(mode_){
        case Mode::Normal:{
            message_="";
            Execute(normal_parser_.Feed(key));
            break;
        }
        case Mode::Insert:{
            HandleInsert(key);
            break;
        }
        case Mode::CommandLine:{
            HandleCommandLine(key);
            break;
        }
        default:{
            //mode_非法??????????????????????????????
            break;
        }
    }
    return;
}

void Editor::Execute(const EditorAction& action) {
    //根据Action的种类调用对应模块
    unsigned long buffercolumn;
    if (buffer_.GetLineAt(window_.GetCursor().row_).empty()) {
        buffercolumn=0;
    }
    else{
        int a{0};
        size_t current = 0;
        for (size_t index = 0; index < buffer_.GetLineAt(window_.GetCursor().row_).size(); ++index) {
            size_t next = (buffer_.GetLineAt(window_.GetCursor().row_)[index]!='\t')? (current+1):(current+4-(current%4));
            if (window_.GetCursor().column_ < next) {
                buffercolumn=index;
                a=1;
                break;
            }
            current = next;
        }
        if(a!=1){
            buffercolumn=buffer_.GetLineAt(window_.GetCursor().row_).size();
        }
    }
    switch (action.kind_) {
        case ActionKind::None:{
            return;
        }
        case ActionKind::Move:{
            //交给Window吧
            window_.ApplyMotion(buffer_,*action.motion_);
            return;
        }
        case ActionKind::InsertBefore:{
            //进入InsertMode,Editor自己就有对应方法
            EnterInsert(window_.GetCursor());
            return;
        }
        case ActionKind::InsertAfter:{
            //从当前Char之后进入InsertMode
            //特判:如果Buffer表示当前Cursor所在的行是空的怎么办?
            if(buffer_.GetLineAt(window_.GetCursor().row_).size()>0){
                if(buffer_.GetLineAt(window_.GetCursor().row_)[buffercolumn]!='\t'){
                    EnterInsert({window_.GetCursor().row_,window_.GetCursor().column_+1});
                }
                else{
                    EnterInsert({window_.GetCursor().row_,window_.GetCursor().column_+4-(window_.GetCursor().column_%4)});
                }
            }
            else{
                EnterInsert({window_.GetCursor().row_,0});
            }
            return;
        }  
        case ActionKind::EnterCommandLine:{
            mode_=Mode::CommandLine;
            message_="";
            command_="";
            return;
        }
        default:{
            //?????????
            break;
        }
    }
}

//Insert模式下Editor对于KeyEvent的处理.
void Editor::HandleInsert(KeyEvent key) {
    //在该函数中你需要同时照顾Buffer和Window的状态
    //1. 若是Escape退出插入模式
    //2. Enter在光标处分行,光标移动到新行开头
    //3. Backspace删除前一个字符;若在行首且不是第一行,则与上一行合并
    //4. 可打印字符和Tab插入当前位置,光标向后移动一列
    //修改内容后记得同步Window中的光标,插入模式允许光标位于line.size()
    //throw std::runtime_error("Not implemented.");
    Position cursorposition{window_.GetCursor()};
    unsigned long buffercolumn;
    if (buffer_.GetLineAt(cursorposition.row_).empty()) {
        buffercolumn=0;
    }
    else{
        int a{0};
        size_t current = 0;
        for (size_t index = 0; index < buffer_.GetLineAt(cursorposition.row_).size(); ++index) {
            size_t next = (buffer_.GetLineAt(cursorposition.row_)[index]!='\t')? (current+1):(current+4-(current%4));
            if (cursorposition.column_ < next) {
                buffercolumn=index;
                a=1;
                break;
            }
            current = next;
        }
        if(a!=1){
            buffercolumn=buffer_.GetLineAt(cursorposition.row_).size();
        }
    }
    unsigned long rowofcursorposition{cursorposition.row_},columnofcursorposition{cursorposition.column_};
    if(key.code_==KeyCode::Escape){
        LeaveInsert();
        return;
    }
    if(key.code_==KeyCode::Enter){
        buffer_.SplitLine(rowofcursorposition,columnofcursorposition);
        window_.SetCursor(buffer_,{rowofcursorposition+1,0},true);
        return;
    }
    if(key.code_==KeyCode::Backspace){
        if(buffercolumn>=1){
            buffer_.EraseCharacter(rowofcursorposition,buffercolumn-1);
            window_.SetCursor(buffer_,{rowofcursorposition,columnofcursorposition-1},true);
        }
        else if(rowofcursorposition>=1){
            std::string rowofbuffer{buffer_.GetLineAt(rowofcursorposition-1)};
            unsigned long column{0};
            for(unsigned long i{0};i+1<=rowofbuffer.size();i++){
                if(rowofbuffer[i]=='\t'){
                    column=column+4-column%4;
                }
                else{
                    column++;
                }
            }
            window_.SetCursor(buffer_,{rowofcursorposition-1,column},true);
            buffer_.JoinLine(rowofcursorposition-1);
        }
    }
    if(key.code_==KeyCode::Character&&IsPrintable(key.value_)){
        buffer_.InsertCharacter(rowofcursorposition,buffercolumn,key.value_);
        window_.SetCursor(buffer_,{rowofcursorposition,columnofcursorposition+1},true);
        return;
    }
    if(key.code_==KeyCode::Character&&key.value_=='\t'){
        buffer_.InsertCharacter(rowofcursorposition,buffercolumn,'\t');
        window_.SetCursor(buffer_,{rowofcursorposition,columnofcursorposition+4-(columnofcursorposition%4)},true);
    }
}

void Editor::EnterInsert(Position position) {
    //切换到Insert模式,设置插入位置并清除旧提示;允许光标停在行尾字符之后
    //throw std::runtime_error("Not implemented.");
    mode_=Mode::Insert;
    message_="";
    window_.SetCursor(buffer_,position,true);
}

void Editor::LeaveInsert() {
    //从插入位置回到Normal模式的字符位置:不在行首时先左移一列,再限制光标范围
    //throw std::runtime_error("Not implemented.");
    mode_=Mode::Normal;
    if(window_.GetCursor().column_>=1){
        window_.SetCursor(buffer_,{window_.GetCursor().row_,window_.GetCursor().column_-1},false);
    }
    else{
        window_.SetCursor(buffer_,window_.GetCursor(),false);
    }
}



void Editor::HandleCommandLine(KeyEvent key) {
    //命令内容保存在command_中,不修改Buffer
    //Escape取消命令,Enter执行命令,Backspace/Delete删除末尾字符,可打印字符追加到末尾
    //注意空命令不能再删除字符
    //throw std::runtime_error("Not implemented.");
    Position cursorposition{window_.GetCursor()};
    unsigned long buffercolumn;
    if (command_.empty()) {
        buffercolumn=0;
    }
    else{
        int a{0};
        size_t current = 0;
        for (size_t index = 0; index < command_.size(); ++index) {
            size_t next = (command_[index]!='\t')? (current+1):(current+4-(current%4));
            if (cursorposition.column_ < next) {
                buffercolumn=index;
                a=1;
                break;
            }
            current = next;
        }
        if(a!=1){
            buffercolumn=command_.size();
        }
    }
    if(key.code_==KeyCode::Escape){
        LeaveCommandLine();
        return;
    }
    if(key.code_==KeyCode::Enter){
        ExecuteCommandLine();
        return;
    }
    if(key.code_==KeyCode::Backspace||key.code_==KeyCode::Delete){
        if(command_.size()>=1){
            command_.pop_back();
        }
        return;
    }
    if (key.code_==KeyCode::Character&&IsPrintable(key.value_)){
        command_.push_back(key.value_);

    }
}

void Editor::ExecuteCommandLine() {
    //1. 保存去掉首尾空白后的命令(用trim),再退出命令行模式,因为退出会清空command_
    //2. 空命令直接返回,否则按第一个空格或Tab拆成命令名和参数
    //3. 在Basic部分中你会发现最后命令就一个命令名,直接根据要求的命令名执行
    //4. 无法识别的命令写入message_,供下一次刷新显示
    //throw std::runtime_error("Not implemented.");
    std::string trimcommand_{Trim(command_)};
    LeaveCommandLine();
    unsigned long lengthoftrimcommand_{trimcommand_.size()};
    if(lengthoftrimcommand_>=1){
        std::string leftcommand_{""},rightcommand_{""};
        unsigned i{0};
        for(;i+1<=lengthoftrimcommand_;i++){
            if(trimcommand_[i]==' '||trimcommand_[i]=='\t'){
                break;
            }
            leftcommand_+=trimcommand_[i];
        }
        i++;
        for(;i+1<=lengthoftrimcommand_;i++){
            rightcommand_+=trimcommand_[i];
        }
        if(leftcommand_=="wq"){
            if(SaveBuffer(rightcommand_)==true){
                running_=false;
                return;
            }
            else{
                //保存异常??????????????????????????????
                return;
            }
        }
        else if(leftcommand_=="q!"||leftcommand_=="quit!"){
            running_=false;
        }
        else{
            message_=trimcommand_;
            //如何下次刷新??????????????????????????????
        }
        return;
    }
    else{
        return;
    }
}

void Editor::LeaveCommandLine() {
    //恢复Normal模式并清空正在输入的命令
    //throw std::runtime_error("Not implemented.");
    mode_=Mode::Normal;
    command_="";
}


bool Editor::SaveBuffer(const std::filesystem::path& path) {
    //1. path为空时调用Save,否则调用SaveAs
    //2. 捕获保存时的异常,把错误写入message_并返回false
    //3. 成功后生成包含文件名和行数的提示,返回true,供wq判断是否可以退出
    //throw std::runtime_error("Not implemented.");
    try{
        if(path.empty()){
            buffer_.Save();
        }
        else{
            buffer_.SaveAs(path);
        }
        unsigned long numberofrows{buffer_.GetLineCount()};
        message_="\""+buffer_.GetDisplayName()+"\" "+std::to_string(numberofrows);
        return true;
    }
    catch(std::exception &e){
        message_=e.what();
        return false;
    }
}

} // namespace sjtu
