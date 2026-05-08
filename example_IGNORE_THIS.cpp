//一个示例的简单通讯录项目，在这里是为了说明设计项目的一个基本思路，忽略这个文件即可。
#include<iostream>
#include<string>
#define MAX_dir 100
using namespace std;

void showmenu(){//你应该先明确项目有哪些功能
    cout<<"------------------\n";
    cout<<"----通讯录管理----\n";
    cout<<"----1.添加人员----\n";
    cout<<"----2.显示人员----\n";
    cout<<"----3.删除人员----\n";
    cout<<"----4.查找人员----\n";
    cout<<"----5.修改人员----\n";
    cout<<"----6.清除人员----\n";
    cout<<"----0.退出系统----\n";
    cout<<"------------------\n";
}

struct Person{//然后你应该开始设计类和结构
    string name;
    int gender;//1.男 2.女
    int age;
    string phone;
    string address;
};

struct Book{
    Person book[MAX_dir];
    int currNum=0;
};

void addPerson(Book* b){//之后再一个个设计各种功能
    if(b->currNum==MAX_dir){cout<<"full dir\n"; return;}
    else{
        string n;
        cout<<"输入姓名\n";
        cin>>n;
        b->book[b->currNum].name=n;

        int g;
        cout<<"输入性别,1=男,2=女\n";
        while(1){
            cin>>g;
            if(g==1||g==2){b->book[b->currNum].gender=g; break;}
            else cout<<"重新输入\n";
        }
        
        int ag;
        cout<<"输入年龄\n";
        while(1){
            cin>>ag;
            if(ag>0) {b->book[b->currNum].age=ag; break;}
            else cout<<"再次输入";
        }

        string tel;
        cout<<"输入电话\n";
        cin>>tel;
        b->book[b->currNum].phone=tel;

        string ad;
        cout<<"输入地址\n";
        cin>>ad;
        b->book[b->currNum].address=ad;

        cout<<"录入完成，点任意键继续\n";
        b->currNum++;
    }
}

void showPerson(Book* b){
    if(b->currNum==0) cout<<"空通讯录\n";
    else{
        for(int i=0;i<b->currNum;i++){
            cout<<b->book[i].name<<"\t"<<b->book[i].age<<"\t"<<b->book[i].phone
            <<"\t"<<b->book[i].address<<"\t"<< (b->book[i].gender==1?"男":"女")<<"\n";
        }
    }
}

int isExist(Book* b,string name){//查找&返回索引
    for(int i=0;i<b->currNum;i++){
        if(b->book[i].name==name) return i;
    }
    return -1;
}

void deletePerson(Book* b){
    cout<<"输入名字\n";
    string name;
    cin>>name;
    int ret=isExist(b,name);
    if(ret==-1){
        cout<<"无此人\n";
        return;
    }
    for(int i=ret;i<b->currNum;i++){
        b->book[i]=b->book[i+1];
    }
    b->currNum--;
    cout<<"已删除"<<name<<"\n";
}

void findPerson(Book* b){
    cout<<"输入姓名";
    string name;
    cin>>name;
    for(int i=0;i<b->currNum;i++){
        if(b->book[i].name==name) cout<<b->book[i].name<<"\t"<<b->book[i].age<<"\t"<<b->book[i].phone
        <<"\t"<<b->book[i].address<<"\t"<< (b->book[i].gender==1?"男":"女")<<"\n";
        return;
    }
    cout<<"无此人\n";
}

void modifyPerson(Book* b){
    cout<<"输入姓名\n";
    string name;
    cin>>name;
    int curr;
    for(curr=0;curr<b->currNum;curr++){
        if(b->book[curr].name==name){
            string opt;
            cout<<"需要修改姓名吗?y/n\n";
            cin>>opt;
            if(opt=="y"){
                string n;
                cout<<"输入姓名\n";
                cin>>n;
                b->book[curr].name=n;
            }

            cout<<"需要修改电话吗?y/n\n";
            cin>>opt;
            if(opt=="y"){
                string tel;
                cout<<"输入电话\n";
                cin>>tel;
                b->book[curr].phone=tel;
            }

            cout<<"需要修改地址吗?y/n\n";
            cin>>opt;
            if(opt=="y"){
                string ad;
                cout<<"输入地址\n";
                cin>>ad;
                b->book[curr].address=ad;
            }

            cout<<"需要修改性别吗?y/n\n";
            cin>>opt;
            if(opt=="y"){
                int gen;
                cout<<"输入性别(1:男,2:女)\n";
                cin>>gen;
                if(gen!=1&&gen!=2){cout<<"无效的性别，不进行修改\n";}//不想写太麻烦，省去了这里的循环
                else{b->book[curr].gender=gen;}
            }

            cout<<"需要修改年龄吗?y/n\n";
            cin>>opt;
            if(opt=="y"){
                int ag;
                cout<<"输入年龄(应该>0)\n";
                cin>>ag;
                if(ag<0){cout<<"无效的年龄，不进行修改\n";}
                else{b->book[curr].age=ag;}
            }
            cout<<"修改完成\n";
            return;
        }
    }
    cout<<"无此人";
}

void clearBook(Book* b){
    b->currNum=0;
    cout<<"已经清空\n";
}



int main(){//可以先把main的框架设计好
    int select;
    Book book;
    while(1){
        showmenu();
        cin>>select;
        switch (select)
        {//每个case尽量简单，复杂的包成一块
        case 0://退出
            cout<<"已退出\n";
            break;

        case 1://添加
            addPerson(&book);
            system("pause");
            break;

        case 2://显示
            showPerson(&book);
            system("pause");
            break;
        
        case 3://删除
            deletePerson(&book);
            system("pause");
            break;

        case 4://查找
            findPerson(&book);
            system("pause");    
            break;
        
        case 5://修改
            modifyPerson(&book);
            system("pause");
            break;

        case 6://清空
            clearBook(&book);
            system("pause");
            break;

        default:
            cout<<"invalid\n";
            break;
        }
        if(select==0) break;
        system("CLS");//运行参数中的DOS命令
    }

    return 0;
}