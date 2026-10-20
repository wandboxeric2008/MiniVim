#include <fstream>

#include "Buffer.hpp"

namespace sjtu {

Buffer::Buffer(const std::filesystem::path& path){
    //throw std::runtime_error("Not implemented.");
    //从path指向的文件构造Buffer,你需要打开文件并且把文件内容填充进Buffer,并正确初始化一些状态.
    //注意path可能为空的边界情况
    path_=path;
    std::ifstream getdocument(path);
    std::string rowofdocument;
    if(std::getline(getdocument,rowofdocument)){
        lines_={rowofdocument};
        while(std::getline(getdocument,rowofdocument)){
            lines_.push_back(rowofdocument);
        }
    }
    else{
        lines_={""};
    }
    return;
}

Buffer::Buffer(std::vector<std::string> lines, std::filesystem::path path) {
    path_=path;
    if(lines.size()>=1){
        lines_=lines;
    }
    else{
        lines_={""};
    }
    return;
}

std::size_t Buffer::GetLineCount() const {
    //返回文件行数
    //throw std::runtime_error("Not implemented.");
    unsigned long numberofrowsoflines_{lines_.size()};
    if(numberofrowsoflines_>=1){
        return numberofrowsoflines_;
    }
    else{
        return 1;
    }
}

const std::string& Buffer::GetLineAt(std::size_t row) const {
    //返回第row行的内容
    //throw std::runtime_error("Not implemented.");
    static const std::string empty{""};
    unsigned long numberofrowsoflines_{lines_.size()};
    if(numberofrowsoflines_>=1){
        if(row+1<=numberofrowsoflines_){
            return lines_[row];
        }
        else{
            //row超范围??????????????????????????????
            return empty;
        }
    }
    else{
        if(row>=1){
            //row超范围??????????????????????????????
            return empty;
        }
        else{
            return empty;
        }
    }
}


std::string Buffer::GetDisplayName() const {
    //返回文件名,若是新文件,返回"[No Name]"
    //throw std::runtime_error("Not implemented.");
    if(!path_.empty()){
        return path_.filename().string();
    }
    else{
        return "[No Name]";
    }
}

bool Buffer::IsModified() const {
    //返回文件和上次保存比起来是否被修改过
    //throw std::runtime_error("Not implemented.");
    if(!path_.empty()){
        std::vector<std::string> originaldocument; 
        std::ifstream getdocument(path_);
        std::string rowofdocument;
        if(std::getline(getdocument,rowofdocument)){
            originaldocument={rowofdocument};
            while(std::getline(getdocument,rowofdocument)){
                originaldocument.push_back(rowofdocument);
            }
        }
        if(originaldocument.size()==0){
            originaldocument={""};
        }
        if(lines_.size()!=0){
            if(lines_!=originaldocument){
                return true;
            }
            else{
                return false;
            }
        }
        else{
            if(originaldocument!=std::vector<std::string>{""}){
                return true;
            }
            else{
                return false;
            }
        }
    }
    else{
        //新文件，无上一版本??????????????????????????????
        return true;
    }
}

void Buffer::InsertCharacter(std::size_t row, std::size_t column, char value) {
    //在第row行第col列插入一个value, 注意越界检查
    //throw std::runtime_error("Not implemented.");
    unsigned long numberofrowsoflines_{lines_.size()};
    if(numberofrowsoflines_>=1){
        if(row+1<=numberofrowsoflines_){
            std::string rowoflines_{lines_[row]};
            unsigned long lengthofrowoflines_{rowoflines_.size()};
            if(column<=lengthofrowoflines_){
                std::string newrow{""};
                long long afterinsert{0};
                for(unsigned long i{0};i<=lengthofrowoflines_;i++){
                    if(i!=column){
                        newrow+=rowoflines_[i+afterinsert];
                    }
                    else{
                        newrow+=value;
                        afterinsert--;
                    }
                }
                lines_[row]=newrow;
                return;
            }
            else{
                //column超范围??????????????????????????????
                return;
            }
        }
        else{
            //row超范围??????????????????????????????
            return;
        }
    }
    else{
        if(row!=0||column!=0){
            //row或column超范围??????????????????????????????
            return;
        }
        else{
            lines_={{value}};
            return;
        }
    }
}

void Buffer::EraseCharacter(std::size_t row, std::size_t column) {
    //在第row行第col列删除一个value
    //throw std::runtime_error("Not implemented.");
    unsigned long numberofrowsoflines_{lines_.size()};
    if(numberofrowsoflines_>=1){
        if(row+1<=numberofrowsoflines_){
            std::string rowoflines_{lines_[row]};
            unsigned long lengthofrowoflines_{rowoflines_.size()};
            if(lengthofrowoflines_>=1){
                if(column+1<=lengthofrowoflines_){
                    std::string newrow{""};
                    unsigned long aftercancel{0};
                    for(unsigned long i{0};i+2<=lengthofrowoflines_;i++){
                        if(i==column){
                            aftercancel++;
                        }
                        newrow+=rowoflines_[i+aftercancel];
                    }
                    lines_[row]=newrow;
                    return;
                }
                else{
                    //column超范围??????????????????????????????
                    return;
                }
            }
            else{
                //无元素可删??????????????????????????????
                return;
            }
        }
        else{
            //row超范围??????????????????????????????
            return;
        }
    }
    else{
        //无元素可删??????????????????????????????
        return;
    }
}

void Buffer::SplitLine(std::size_t row, std::size_t column) {
    //在第row行第col列分割,即在此处敲了回车键
    //throw std::runtime_error("Not implemented.");
    unsigned long numberofrowsoflines_{lines_.size()};
    if(numberofrowsoflines_>=1){
        if(row+1<=numberofrowsoflines_){
            std::string rowoflines_{lines_[row]},insertthis{""};
            unsigned long lengthofrowoflines_{rowoflines_.size()},curcol{0},i{0},stop{0};
            if(column==0){
                insertthis=lines_[row];
                lines_[row]="";
            }
            else{
                for(;i+1<=lengthofrowoflines_;i++){
                    if(curcol<column){
                        if(i==0){
                            stop=0;
                        }
                        else if(i>stop){
                            stop=i;
                        }
                    }
                    if(rowoflines_[i]=='\t'){
                        curcol=curcol+4-curcol%4;
                    }
                    else{
                        curcol++;
                    }
                }
                std::string newrow{""};
                unsigned long j{0};
                for(;j<=stop;j++){
                    newrow+=rowoflines_[j];
                }
                for(;j+1<=lengthofrowoflines_;j++){
                    insertthis+=rowoflines_[j];
                }
                lines_[row]=newrow;
            }
            std::vector<std::string> newdocument{};
            long long afterinsert{0};
            for(unsigned long i{0};i<=numberofrowsoflines_;i++){
                if(i!=row+1){
                    newdocument.push_back(lines_[i+afterinsert]);
                }
                else{
                    newdocument.push_back(insertthis);
                    afterinsert--;
                }
            }
            lines_=newdocument;
            return;
        }
        else{
            //row超范围??????????????????????????????
            return;
        }
    }
    else{
        if(row>=1||column>=1){
            //row或column超范围??????????????????????????????
            return;
        }
        else{
            lines_={"",""};
            return;
        }
    }
}

void Buffer::JoinLine(std::size_t row) {
    //把第row + 1行合并进第row行
    //throw std::runtime_error("Not implemented.");
    unsigned long numberofrowsoflines_{lines_.size()};
    if(numberofrowsoflines_>=2){
        if(row+2<=numberofrowsoflines_){
            lines_[row]+=lines_[row+1];
            for(unsigned long i{row+1};i+2<=numberofrowsoflines_;i++){
                lines_[i]=lines_[i+1];
            }
            lines_.pop_back();
            return;
        }
        else{
            //row+1超范围??????????????????????????????
            return;
        }
    }
    else{
        //行数不足??????????????????????????????
        return;
    }
    
}

void Buffer::Save() {
    //把文件内容保存, 直接调用WriteTo方法
    //throw std::runtime_error("Not implemented.");
    WriteTo(path_);
    return;
}

void Buffer::SaveAs(const std::filesystem::path& path) {
    //throw std::runtime_error("Not implemented.");
    WriteTo(path);
    return;
}


void Buffer::WriteTo(const std::filesystem::path& path) const {
    //实际将缓冲区中的内容写入path指向的文件中
    //throw std::runtime_error("Not implemented.");
    std::ofstream setdocument(path);
    unsigned long numberofrowsoflines_{lines_.size()};
    if(numberofrowsoflines_>=1){
        for(unsigned long i{0};i+1<=numberofrowsoflines_;i++){
            setdocument<<lines_[i];
            setdocument<<"\n";
        }
    }
    else{
        setdocument<<"";
    }
    return;
}

} // namespace sjtu