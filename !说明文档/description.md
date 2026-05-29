# 小型医院医疗管理系统——答辩稿

---

## 一、项目概述

本系统是一个基于 C++ 开发的轻量级医疗管理系统，面向小型医院设计。系统采用**链表**作为核心数据结构，12 类实体之间通过 **string 类型的 ID** 实现关联，支持管理员、医生、患者三种角色，涵盖挂号、门诊、检查、处方、住院、药房管理及多维度报表等完整功能。

**技术指标**：
- 代码量：约 3500 行 C++
- 12 个实体类型，全部用单向链表管理
- 默认数据：5 科室、22 医生、125 患者、25 药品、7 病房
- 编译：`g++ -std=c++11 -O2`，零错误，零崩溃路径

---

## 二、系统架构

### 2.1 文件结构

```
├── global.h / global.cpp      全局变量、常量、时间/资金管理
├── entities.h                 12 个实体结构体定义
├── list_ops.h / list_ops.cpp  链表通用操作 + ID 生成
├── utils.h / utils.cpp        鲁棒输入/输出、辅助函数
├── init_data.h / init_data.cpp 默认数据初始化
├── persistence.h / persistence.cpp  数据持久化（保存/加载）
├── batch_input.h / batch_input.cpp  批量数据导入（txt/标准输入）
├── menus_admin.h / menus_admin.cpp  管理员界面
├── menus_doctor.h / menus_doctor.cpp 医生界面
├── menus_patient.h / menus_patient.cpp 患者界面
├── reports.h / reports.cpp    报表查询
└── main.cpp                   入口、主循环、内存清理
```

### 2.2 核心设计原则

**1. 链表存储**：所有实体数据存储在单向链表中，每个实体结构体包含一个 `next` 指针。链表的增删改查通过 `list_ops.cpp` 中的通用函数完成。系统在初始化时遍历链表统计数量，在整个运行期间通过遍历实现查询和筛选。

**2. ID 关联**：实体之间的关联通过 string 类型的 ID 实现，避免 C++ 复杂的指针管理。例如医生的 `departmentID` 字段存储科室的 ID，需要获取科室信息时通过 `findDepartmentByID` 查找。这种设计使得实体之间的耦合度极低，删除或修改一个实体不会直接影响关联实体。

**3. 全局时钟**：系统使用 `globalTime`（unsigned long long，秒）模拟时间流逝。`day` 由 `globalTime / 86400 + 1` 自动计算，表示绝对天数（Day 1, Day 2, ...）。`week` 由 `((day - 1) % 7) + 1` 计算，表示星期几（1=周一..7=周日）。`setTime(unsigned long long newTime)` 是时间的唯一修改入口，同时更新 globalTime、day、week，确保数据一致性。

**4. 资金管理**：`money`（double）跟踪医院资金。根据设计规格，money 只受三项收入影响：挂号费（20元/次）、药品费（处方总价）、住院费（200元/天）。在时间推进时，住院费每日自动从押金扣除并加入医院资金；在患者缴费时，挂号费和药费加入医院资金；出院时不再重复计费（每日扣费已在时间推进中完成）。

---

## 三、全局基础设施（global.h / global.cpp）

### 3.1 全局变量

| 变量 | 类型 | 说明 |
|------|------|------|
| `globalTime` | unsigned long long | 全局秒数，0 表示 Day1 00:00:00 |
| `day` | unsigned int | 绝对天数，由公式 `globalTime / 86400 + 1` 计算 |
| `week` | unsigned int | 星期几，由公式 `((day - 1) % 7) + 1` 计算（1=周一..7=周日） |
| `money` | double | 医院资金，所有金额操作通过 `round2()` 保留小数点后2位 |

这四个全局变量的定义位于 `global.cpp`，声明在 `global.h`。其他模块通过 `extern` 关键字引用它们。

### 3.2 关键函数实现

**`setTime(unsigned long long newTime)`**（global.cpp）：
```cpp
void setTime(unsigned long long newTime) {
    globalTime = newTime;
    day = (unsigned int)(newTime / 86400) + 1;
    week = ((day - 1) % 7) + 1;
}
```
这是时间的唯一修改入口，确保 day 和 week 始终由 globalTime 计算得出。该函数在时间推进和初始化时被调用。向下取整的特性意味着：如果只推进 3600 秒（1小时），day 不变，不会触发住院费扣减。

**`formatTime()`**（global.h，内联函数）：
```cpp
inline std::string formatTime() {
    unsigned int seconds = globalTime % 60;
    unsigned int minutes = (globalTime / 60) % 60;
    unsigned int hours = (globalTime / 3600) % 24;
    // snprintf 格式化为 "DayX HH:MM:SS"
}
```
将秒数转换为时分秒显示。取模运算 `% 60` 得到秒数，除60再取模得到分钟数，除3600再取模24得到小时数。配合 `day` 组合成 `DayX HH:MM:SS` 格式。

**`round2(double val)`**（global.h，内联函数）：
```cpp
inline double round2(double val) {
    return std::round(val * 100.0) / 100.0;
}
```
使用 `std::round` 实现四舍五入，防止浮点数运算时的微小误差累积。在所有涉及金额的加减操作后都调用此函数。例如：`money = round2(money + 200.00)`。

### 3.3 常量定义

```cpp
MAX_NAME_LEN = 20        // 姓名最大字符数，符合小型医院患者姓名长度
MAX_BEDS = 30            // 病房最大床位数
MAX_WORKDAYS = 7         // 一周天数
REGISTRATION_FEE = 20.00 // 挂号费固定价格
HOSPITAL_DAILY_FEE = 200.00 // 每日住院费
SECONDS_PER_DAY = 86400  // 一天秒数常量
```

### 3.4 ID前缀体系

为了区分不同类型的实体，每类实体使用不同的ID前缀：
```
DEPT  → 科室ID    如 DEPT001
DOC   → 医生工号  如 DOC022
PAT   → 患者ID    如 PAT125
R-    → 挂号ID    如 R-0001-016 (Day1第16号)
CS    → 就诊ID    如 CS0001
EX    → 检查ID    如 EX0001
PR    → 处方ID    如 PR0001
MED   → 药品ID    如 MED025
WRD   → 病房ID    如 WRD007
HSP-  → 住院ID    如 HSP-0001-003
ADM   → 管理员ID
```

挂号ID和住院ID的格式中嵌入了日期信息（`R-YYYY-序号`），便于从ID直接识别创建日期。

### 3.5 状态码体系

系统设计了三套状态码分别对应不同实体：

| 实体类型 | 常量 | 值 | 含义 |
|----------|------|-----|------|
| 挂号 | STATUS_PENDING | 0 | 待就诊 |
| 挂号 | STATUS_SEEN | 1 | 已就诊 |
| 挂号 | STATUS_EXPIRED | 2 | 已过期 |
| 看诊/检查 | STATUS_VALID | 0 | 有效 |
| 看诊/检查 | STATUS_CANCELLED | 1 | 已撤销 |
| 处方 | (字面量) | 0 | 待缴费 |
| 处方 | (字面量) | 1 | 已撤销 |
| 处方 | (字面量) | 2 | 已缴费 |
| 住院 | (字面量) | 0 | 在院 |
| 住院 | (字面量) | 1 | 已出院 |
| 住院 | STATUS_CANCELLED | 1 | 已撤销 |

通过 `getRegStatusStr()`、`getPrescStatusStr()`、`getRecordStatusStr()`、`getHospStatusStr()` 等函数将状态码转换为中文字符串 "待就诊"、"已缴费"、"在院" 等，用于界面显示。

---

## 四、实体设计（entities.h）

系统定义了 12 个实体结构体，每个都包含 next 指针用于链表链接。实体之间的关联通过 ID 字符串实现，而非指针，避免了复杂的指针管理。

### 4.1 DepartmentNode（科室）

```cpp
struct DepartmentNode {
    string deptID;       // 唯一标识，如 DEPT001
    string name;         // 科室名称，长度≤20
    string description;  // 描述文本，长度≤100
    DepartmentNode* next;
};
```

科室是整个系统的组织核心，医生通过 departmentID 关联到科室，病房通过 departmentID 关联到科室，药品通过 DeptMedicineNode 间接关联到科室。

### 4.2 DoctorNode（医生）

```cpp
struct DoctorNode {
    string doctorID;     // 工号，如 DOC001
    string name;         // 姓名
    int level;           // 0=主任/1=副主任/2=主治/3=住院
    string departmentID; // 所属科室ID（关联到 Department）
    int workDays[7];     // 出诊星期数组（1=周一~7=周日）
    int workDayCount;    // 实际出诊天数
    DoctorNode* next;
};
```

医生的 `workDays[]` 是一个定长数组（大小7），用 `workDayCount` 跟踪实际填入了几个出诊日。`doctorWorksOnDay(DoctorNode*, int dayOfWeek)` 函数遍历这个数组检查指定星期是否出诊。系统默认的22名医生分布在5个科室，每个医生有不同的级别和出诊安排。

### 4.3 PatientNode（患者）——门诊/住院统一管理

```cpp
struct PatientNode {
    string patientID;    // 唯一标识
    string name;         // 姓名
    int age;             // 年龄
    string contact;      // 联系方式
    int type;            // 0=门诊患者 / 1=住院患者
    double deposit;      // 住院押金余额（仅住院患者有效）
    int admitDays;       // 已住院天数
    string bedID;        // 所在床位标识
    PatientNode* next;
};
```

采用**统一链表**管理门诊和住院两种患者，通过 `type` 字段区分。这种设计简化了患者管理——查找患者时只需遍历一条链表。住院患者额外有押金、住院天数和床位信息。系统默认包含 90 名门诊患者和 35 名住院患者。

### 4.4 RegistrationNode（挂号记录）

```cpp
struct RegistrationNode {
    string regID;         // 编号 R-YYMMDD-序号
    string patientID;     // 患者ID
    string departmentID;  // 科室ID
    string doctorID;      // 医生ID
    unsigned int regDay;  // 挂号时的绝对天数
    int status;           // 0=待就诊/1=已就诊/2=已过期/3=已撤销
    RegistrationNode* next;
};
```

挂号记录是患者进入诊疗流程的入口。一条挂号记录同时关联患者、科室、医生三方。挂号时系统检查：(1) 医生当日是否出诊 (2) 同一患者+同一科室+同一天 是否已有未处理的挂号。

### 4.5 ConsultationNode（看诊记录）

```cpp
struct ConsultationNode {
    string consultID;       // 就诊ID
    string regID;           // 关联的挂号ID（一对一）
    string patientID;
    string doctorID;
    string complaint;       // 主诉——患者症状描述
    string diagnosis;       // 诊断——医生结论
    unsigned int consultDay;
    int status;             // 0=有效/1=已撤销
    ConsultationNode* next;
};
```

一次挂号对应至多一条看诊记录。看诊记录由医生接诊时创建，包含主诉和诊断两个关键医疗信息字段。`regID` 关联回挂号记录。

### 4.6 ExaminationNode（检查记录）

```cpp
struct ExaminationNode {
    string examID;        // 检查ID
    string consultID;     // 关联看诊ID（可为空，住院检查不关联看诊）
    string patientID;
    string itemName;      // 检查项目名称
    double cost;          // 检查费用
    unsigned int examDay;
    int status;
    ExaminationNode* next;
};
```

### 4.7 PrescriptionNode（处方记录）

```cpp
struct PrescriptionNode {
    string prescID;       // 处方ID
    string consultID;     // 关联看诊ID
    string patientID;
    string doctorID;
    double totalAmount;   // 处方总金额
    unsigned int prescDay;
    int status;           // 0=待缴费/1=已撤销/2=已缴费
    PrescriptionNode* next;
};
```

开处方时不扣库存，仅在患者缴费后扣减。处方的药品明细存储在独立的 PrescMedicineNode 链表中。

### 4.8 MedicineNode（药品）

```cpp
struct MedicineNode {
    string medID;         // 药品ID
    string tradeName;     // 商品名
    string genericName;   // 通用名
    string alias;         // 别名
    string spec;          // 规格
    int stock;            // 当前库存
    int consumed;         // 累计消耗量（出库统计）
    MedicineNode* next;
};
```

药品的 stock 受入库（增加）和缴费后扣减（减少）双重影响。consumed 是累计值，用于统计消耗量。

### 4.9 PrescMedicineNode（处方-药品关联）

```cpp
struct PrescMedicineNode {
    string prescID;       // 处方ID
    string medID;         // 药品ID
    int quantity;         // 数量
    double unitPrice;     // 单价
    PrescMedicineNode* next;
};
```

这是实现多对多关联的关键：一个处方包含多种药品（多个 PrescMedicineNode 节点共享同一个 prescID），一种药品可以出现在多个处方中。系统通过遍历此链表并匹配 prescID 来获取处方的药品明细。

### 4.10 DeptMedicineNode（科室-药品关联）

```cpp
struct DeptMedicineNode {
    string deptID;        // 科室ID
    string medID;         // 药品ID
    DeptMedicineNode* next;
};
```

医生开处方时，系统通过此链表校验所选药品是否属于该医生所在科室。系统初始化时建立了60条科室药品关联。

### 4.11 WardNode（病房）

```cpp
struct WardNode {
    string wardID;        // 病房ID
    int type;             // 0=普通/1=特殊
    string departmentID;  // 所属科室ID
    int bedCount;         // 床位数（≤30）
    string bedStatus[MAX_BEDS]; // 每个床位的状态（空串=空闲，患者ID=已占用）
    WardNode* next;
};
```

采用**定长数组**方式管理床位：`bedStatus[i]` 存储占用该床位的患者ID，空字符串表示空闲。操作通过 `getAllocateBed`、`releaseBed`、`getFreeBedCount` 三个专用函数完成。

### 4.12 HospitalizationNode（住院记录）

```cpp
struct HospitalizationNode {
    string hospID;        // 住院ID HSP-YYYY-序号
    string patientID;
    string wardID;
    int bedNo;            // 床位号（0-indexed）
    string doctorIDs[10]; // 主管医生数组
    int doctorCount;      // 主管医生数量
    unsigned int admitDay;    // 入院日期
    unsigned int dischargeDay; // 出院日期（0=未出院）
    double deposit;       // 押金余额
    int status;           // 0=在院/1=已出院/2=已撤销
    HospitalizationNode* next;
};
```

医生与住院患者之间是一个多对多关系（一名患者可以有多个主管医生，一名医生可以负责多个住院患者），通过 `doctorIDs[]` 数组 + `doctorCount` 实现。床位号 `bedNo` 是0索引的（内部使用），显示时+1变为1索引。

### 4.13 AdminNode（管理员）

```cpp
struct AdminNode {
    string adminID;       // 管理员ID
    string password;      // 密码
    AdminNode* next;
};
```

系统默认创建了两个管理员：admin/123456 和 root/root123。

---

## 五、链表操作层（list_ops.h / list_ops.cpp）

这是系统的数据访问层。它为每种实体提供了标准化的链表操作函数，所有函数都是类型安全的。

### 5.1 通用操作模式（以 Department 为例）

**三种插入方式——体现链表灵活性**：

- **`insertDepartmentHead(DepartmentNode*& head, DepartmentNode* node)`**：头插法。将新节点插入链表头部：`node->next = head; head = node;`。时间复杂度 O(1)。用于不需要排序的场景，如新增科室、患者、药品。

- **`insertDepartmentTail(DepartmentNode*& head, DepartmentNode* node)`**：尾插法。遍历到链表尾部：`while(cur->next) cur = cur->next; cur->next = node;`。时间复杂度 O(n)。用于需要保持插入顺序的场景，如挂号记录（先挂号的先看诊）。

- **`insertDepartmentSorted(DepartmentNode*& head, DepartmentNode* node)`**：字典序插入。比较 `deptID` 字符串，找到插入位置：`while(cur->next && cur->next->deptID < node->deptID) cur = cur->next;`。用于需要按ID排序的场景，如医生列表。

**删除操作——体现链表的节点摘除**：

- **`deleteDepartmentByID(DepartmentNode*& head, const string& id)`**：物理删除。实现分为两种情况：(1) 删除头节点：保存 `head` 到临时变量，`head = head->next`，`delete` 原头节点。(2) 删除中间/尾节点：遍历链表找到目标节点的前驱，修改前驱的 `next` 指针跳过目标节点，然后 `delete` 目标节点。时间复杂度 O(n)。

**查找操作——链表遍历**：

- **`findDepartmentByID(DepartmentNode* head, const string& id)`**：从头节点开始遍历链表，逐个比较 `cur->deptID == id`。找到则返回节点指针，未找到返回 `nullptr`。所有调用者在使用返回值前必须检查非空。

**统计操作**：

- **`countDepartments(DepartmentNode* head)`**：遍历链表并计数 `cnt++`，O(n)。

**清理操作**：

- **`freeDepartmentList(DepartmentNode*& head)`**：遍历链表，逐个 `delete` 节点释放内存。在程序退出时调用，防止内存泄漏。

### 5.2 每种实体的函数组

对于 Department、Doctor、Patient、Medicine 四种需要排序的实体，提供 Head/Tail/Sorted 三种插入方式。

对于 Registration、Consultation、Examination、Prescription、PrescMedicine、DeptMedicine、Ward、Hospitalization、Admin 等实体，提供 Head/Tail 两种插入方式，无 Sorted 版本。

所有实体都有 `deleteByID`、`findByID`、`freeList` 操作。

### 5.3 特殊操作详解

**床位管理（WardNode）**：

`getFreeBedCount(WardNode* ward)`：遍历 `bedStatus[i]` 数组，统计空字符串的数量。返回可用床位数。

`getAllocateBed(WardNode* ward, const string& patientID)`：从头遍历 `bedStatus[i]`，找到第一个空字符串位置，将其赋值为 `patientID` 表示占用。返回分配的床位号（0索引）。如果没有空闲床位，返回 -1。这个函数在系统初始化时为住院患者分配床位。

`releaseBed(WardNode* ward, int bedNo)`：将指定床位的 `bedStatus[bedNo]` 清空为空字符串。先检查 `bedNo` 的合法性（`bedNo >= 0 && bedNo < ward->bedCount`）。

**住院患者查找**：

`findActiveHospitalizationByPatient(HospitalizationNode* head, const string& patientID)`：遍历链表，同时检查 `patientID` 匹配 且 `status == 0`（在院状态）。这比普通的 `findByID` 多了一个状态条件。

**科室药品关联操作**：

`findDeptMedicine(DeptMedicineNode* head, const string& deptID, const string& medID)`：双条件匹配查找——遍历链表，同时比较 `deptID` 和 `medID` 两个字段。用于检查科室药品关联是否已存在（防重复）和验证药品是否属于指定科室。

`deleteDeptMedicine(DeptMedicineNode*& head, const string& deptID, const string& medID)`：双条件匹配删除。遍历找到同时满足两个条件的节点后从链表中摘除。

### 5.4 ID 生成系统

10 个全局计数器定义在 `list_ops.cpp` 中：
```cpp
int nextDeptID = 1;     int nextDoctorID = 1;
int nextPatientID = 1;  int nextRegID = 1;
int nextConsultID = 1;  int nextExamID = 1;
int nextPrescID = 1;    int nextMedID = 1;
int nextWardID = 1;     int nextHospID = 1;
```

对应的 `generateXxxID()` 函数使用 `snprintf` 格式化ID字符串并后置递增计数器。例如：
```cpp
string generateDeptID() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%03d", PREFIX_DEPT, nextDeptID++);
    return string(buf);
}
```

挂号ID和住院ID额外接受一个 `unsigned int day` 参数，将日期信息嵌入ID中：
```cpp
string generateRegID(unsigned int day) {
    char buf[32];
    snprintf(buf, sizeof(buf), "R-%04d-%03d", day, nextRegID++);
    return string(buf);
}
```

**`updateIDCountersFromLists()`** 函数在从文件加载数据后调用。它遍历每条链表，通过 `extractNum()` 函数提取每个ID中的数字部分，找出最大值，并将对应的计数器设置为 `最大值+1`。这确保了加载后新生成的ID不会与已有ID冲突。`extractNum()` 的实现使用了 `rfind('-')` 找到最后一个连字符，`atoi` 提取序号。

---

## 六、鲁棒输入/输出（utils.h / utils.cpp）

### 6.1 输入函数——崩溃防御的第一道防线

**`readInt(const char* prompt)`**：
```cpp
int readInt(const char* prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {        // 尝试读取整数
            clearInputBuffer();  // 清除残留换行符
            return val;          // 成功：返回
        }
        // 失败：cin >> val 返回 false（如用户输入了 "abc"）
        cin.clear();             // 清除 cin 的 failbit
        clearInputBuffer();      // 丢弃错误输入
        cout << "[错误] 输入不是有效整数，请重新输入。" << endl;
        // 循环重新提示
    }
}
```

这个函数的核心思想是 **cin 的流状态管理**：当用户输入无法解析为目标类型时，`cin >> val` 会将 `cin` 置为 fail 状态。必须通过 `cin.clear()` 清除错误标志，通过 `cin.ignore()` 丢弃缓冲区中的错误数据，否则后续任何 cin 操作都会立即失败。函数使用 `while(true)` 循环，只有成功读取才 return，**保证永远返回一个合法值**。

**`readDouble()`**：与 readInt 相同模式，用于金额和浮点数输入。在读取成功时自动调用 `round2()` 保留两位小数。

**`readString(const char* prompt, int maxLen)`**：
```cpp
string readString(const char* prompt, int maxLen) {
    string input;
    while (true) {
        cout << prompt;
        if (getline(cin, input)) {
            // 去除首尾空白字符
            size_t start = input.find_first_not_of(" \t\r");
            size_t end = input.find_last_not_of(" \t\r");
            input = input.substr(start, end - start + 1);
            // 校验非空和长度
            if (input.empty()) { /* 提示重新输入 */ continue; }
            if (input.length() > maxLen) { /* 提示超长 */ continue; }
            return input;
        }
    }
}
```

使用 `getline` 读取整行（包括中文等宽字符），然后用 `substr` 去除首尾空白（空格、制表符、回车）。对空输入和超长输入分别提示并重试。

**`readOptionalString()`**：与 readString 相同，但允许返回空字符串。用于"可留空"的字段，如药品别名、科室描述。

**`readConfirm(const char* prompt)`**：调用 `readString` 读取输入，检查是否为 y/yes/是 或 n/no/否。用于所有删除/修改/出院等关键操作的确认。

**`readIntRange(const char* prompt, int min, int max)`**：基于 `readInt` 再叠加范围检查。超出范围则提示并重试。用于年龄（0-150）、病床数（1-30）、级别（0-3）等有限范围输入。

**`readNonNegDouble(const char* prompt)`**：基于 `readDouble` 再检查 `>= 0.0`。用于金额、押金等不能为负的输入。

**`clearInputBuffer()`**：
```cpp
void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
```
使用 `cin.ignore` 丢弃输入缓冲区中直到下一个换行符（包括换行符本身）的所有字符。

### 6.2 输出/显示函数

**`printLine(int len, char ch)`**：循环打印 len 个指定字符（默认 '-' 60个），用于界面分隔线。

**`printTitle(const char* title)`**：打印居中标题，两边用 `=` 线包围。

**`printStatus()`**：调用 `formatTime()` 和 `money` 显示当前时间和医院资金。在管理界面顶部显示，让用户随时了解系统状态。

**`pauseScreen(const char* msg)`**：
```cpp
void pauseScreen(const char* msg) {
    cout << msg;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
```
暂停程序，等待用户按回车键继续。`cin.ignore` 会等待用户输入一个换行符。

### 6.3 校验函数——业务规则验证

**`doctorWorksOnDay(DoctorNode* doctor, int dayOfWeek)`**：遍历医生的 `workDays[]` 数组，检查是否包含指定的星期值。

**`deptExists(DepartmentNode* head, const string& deptID)`**：包装 `findDepartmentByID`，返回 bool。

**`isMedicineInDept(DeptMedicineNode* dmHead, const string& deptID, const string& medID)`**：包装 `findDeptMedicine`，返回 bool。

### 6.4 状态转换函数

将整数状态码转换为中文字符串。例如 `getRegStatusStr(int status)` 将 0→"待就诊"、1→"已就诊"、2→"已过期"、3→"已撤销"。

---

## 七、默认数据初始化（init_data.h / init_data.cpp）

`initDefaultData()` 是整个系统的数据基础，约450行代码，在首次启动或存档加载失败时执行。

### 7.1 初始化顺序与数据构成

初始化遵循**从无依赖实体到有依赖实体**的顺序，确保创建关联记录时被引用的实体已经存在：

**第1步：管理员（2个）**
```
admin / 123456
root  / root123
```
通过 `insertAdminTail` 插入。

**第2步：科室（5个）**——使用辅助函数 `makeDept(name, desc)` 创建
```
DEPT001 内科    - 诊治内科疾病，包含心血管、呼吸、消化等
DEPT002 外科    - 诊治外科疾病，包含普外、骨外、神经外等
DEPT003 儿科    - 诊治儿童疾病及儿童保健
DEPT004 妇产科  - 诊治妇产科疾病及孕产妇保健
DEPT005 急诊科  - 处理急危重症患者的紧急救治
```
科室名称和描述通过硬编码数组定义。每个科室在创建时调用 `generateDeptID()` 自动生成序号。

**第3步：医生（22名）**——分布在5个科室
```
内科 5名：张伟明(主任) 李芳华(副主任) 王建国(主治) 陈晓燕(主治) 刘志强(住院)
外科 5名：赵大明(主任) 孙丽萍(副主任) 周建华(主治) 吴国强(主治) 郑晓峰(住院)
儿科 4名：冯小梅(主任) 陈玉兰(副主任) 褚明辉(主治) 卫思远(住院)
妇产科4名：蒋美玲(主任) 沈秋菊(副主任) 韩秀丽(主治) 杨丽华(住院)
急诊科4名：朱志刚(主任) 秦海峰(副主任) 尤建军(主治) 许文斌(住院)
```
每名医生通过 `makeDoctor(name, level, deptID, workDays, count)` 创建，其中 `workDays` 数组定义了医生的出诊星期。例如张伟明主任的工作日为 1,3,5（周一三五），朱志刚主任在急诊科的工作日为 1,2,3,4,5,6（周一到周六）。

**第4步：药品（25种）**——涵盖6大类别
```
抗生素类：阿莫西林、头孢克洛、青霉素钠、甲硝唑
解热镇痛：布洛芬、对乙酰氨基酚、阿司匹林
消化系统：奥美拉唑、蒙脱石散
心血管：硝苯地平、氯沙坦钾、阿托伐他汀、硝酸甘油
糖尿病：二甲双胍、胰岛素注射液
注射液类：葡萄糖、氯化钠、维生素C、地塞米松、缩宫素
其他：复方甘草片、氨溴索、氯雷他定、小儿氨酚黄那敏
```
每种药品的库存量从100到1200不等（如氯化钠注射液1200瓶、缩宫素注射液100支），通过 `makeMed()` 创建。

**第5步：科室-药品关联（60条）**——通过 DeptMedicineNode 链表建立
```
内科(13种)：阿莫西林、布洛芬、对乙酰氨基酚、奥美拉唑、蒙脱石散、
          复方甘草片、硝苯地平、氯沙坦钾、二甲双胍、胰岛素、
          阿司匹林、阿托伐他汀、硝酸甘油
外科(12种)：...（不同的药品组合）
```
每个科室关联的药品组合反映了该科室的用药特点。例如儿科关联了小儿氨酚黄那敏颗粒，妇产科关联了缩宫素注射液。

**第6步：病房（7个）**——分布在5个科室
```
内科：WRD001 普通病房 30床
外科：WRD002 特需病房 15床 + WRD003 普通病房 30床
儿科：WRD004 普通病房 30床
妇产科：WRD005 特需病房 20床 + WRD006 普通病房 20床
急诊科：WRD007 普通病房 10床
```

**第7步：患者（125名）**——90门诊 + 35住院
```
门诊患者：通过数组 outNames[90] 批量创建
住院患者：通过数组 inNames[35] 批量创建
```
患者的年龄通过 `18 + (i*7) % 70` 公式生成（范围18-87岁）。

**第8步：住院记录（30条）**——为前30名住院患者创建
```
流程：遍历患者链表 → 找到 type==1 的患者 →
      循环尝试6个病房数组 → getAllocateBed 分配床位 →
      从对应科室选择1-2名医生 → 创建 HospitalizationNode
      押金随机: 3000 + rand()%10000
```

**第9步：样本诊疗数据**——15条挂号 + 3条看诊 + 3张处方 + 1条检查
```
15条挂号 → 取前10名患者的 data，其中3条标记为已就诊
3条看诊 → 关联到前3条已就诊的挂号
3张处方 → 每张关联1-2种药品，总金额15-35元
1条检查 → "血常规检查"，50元
```

### 7.2 辅助函数

```cpp
static DepartmentNode* makeDept(name, desc)   → new DepartmentNode + generateDeptID
static DoctorNode* makeDoctor(name, level, deptID, workdays[], count) → new DoctorNode + generateDoctorID
static PatientNode* makePatient(name, age, contact, type) → new PatientNode + generatePatientID
static MedicineNode* makeMed(tradeName, genericName, alias, spec, stock) → new MedicineNode + generateMedID
static WardNode* makeWard(type, deptID, bedCount) → new WardNode + generateWardID
```

这些辅助函数封装了节点创建和ID生成的细节，使初始化代码清晰简洁。

---

## 八、数据持久化（persistence.h / persistence.cpp）

### 8.1 保存机制——saveAllData()

`saveAllData()` 将所有13种链表数据序列化到14个文本文件。每个文件的每行是一条记录，字段之间用 `|` 分隔。全局状态单独存储在一个文件中。

**字段转义**：`esc()` 函数处理字符串中的特殊字符。由于使用 `|` 作为字段分隔符，字符串中一旦出现 `|` 就会破坏数据结构。因此：
- `|` 转义为 `\|`
- `\` 转义为 `\\`
- 换行符转义为 `\n`

```cpp
static string esc(const string& s) {
    string r;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == '|') r += "\\|";
        else if (s[i] == '\\') r += "\\\\";
        else if (s[i] == '\n') r += "\\n";
        else r += s[i];
    }
    return r;
}
```

**全局状态文件**`save_global.txt` 格式：
```
globalTime day week money
nextDeptID nextDoctorID ... nextHospID
```
第一行是四个全局变量值，第二行是10个ID计数器值。保存时同时保存了 ID 计数器的状态，加载后能正确恢复 ID 生成序列。

**实体文件格式**（以 Department 为例，`save_department.txt`）：
```
DEPT001|内科|诊治内科疾病，包含心血管、呼吸、消化等
DEPT002|外科|诊治外科疾病，包含普外、骨外、神经外等
```
医生文件还包含出诊日字段（逗号分隔）、患者文件包含押金和住院天数等特殊字段。

**病房文件**的格式包含动态长度的床位状态：
```
WRD001|0|DEPT001|30|||||PAT080|...|  （30个床位状态，空=空闲，患者ID=已占用）
```

### 8.2 加载机制——loadAllData()

`loadAllData()` 按顺序加载所有文件：

1. **加载全局状态**：打开 `save_global.txt`，读取第一行的 `globalTime`、`savedDay`、`savedWeek`、`money`。然后读取第二行的 ID 计数器。关键操作：读取后调用 `setTime(globalTime)` 用 setTime 重新计算 day 和 week（而非使用文件中存储的值），确保 day 和 week 始终由 globalTime 推导的正确性。

2. **加载科室**：`splitLine(line, '|')` 解析每行为三个字段，创建 DepartmentNode 并尾插入链表。

3. **加载医生**：解析出诊日字段（`parseWorkDays` 函数用 `stringstream + getline(token, ',')` 分割逗号分隔的工作日列表）。

4. **加载患者/挂号/看诊/检查/处方/药品**：同上模式。

5. **加载处方药品关联**：恢复 PrescMedicineNode 链表。

6. **加载科室药品关联**：恢复 DeptMedicineNode 链表。

7. **加载病房**：恢复 WardNode，包括动态长度的 `bedStatus[]` 数组（通过 for 循环 `parts[4+i]` 读取）。

8. **加载住院记录**：恢复 HospitalizationNode，包括 `doctorIDs[]` 数组（通过 `parseDoctorIDs` 函数解析）。

9. **加载管理员**：恢复 AdminNode。

**splitLine 函数的内嵌反转义**：
```cpp
static vector<string> splitLine(const string& line, char delim) {
    // ...
    if (escape) {
        if (line[i] == '|') current += '|';    // \| → |
        else if (line[i] == '\\') current += '\\'; // \\ → \
        else if (line[i] == 'n') current += '\n';  // \n → 换行
        else current += line[i];
        escape = false;
    }
    // ...
}
```
这个内嵌的转义处理确保保存时的 `\|` 在加载时还原为 `|`，`\\` 还原为 `\`，`\n` 还原为换行符。**注意**：所有转义和反转义都在 splitLine 内部完成，不需要单独调用 unesc 函数。

**错误处理**：任何文件打开失败、字段数量不足等异常都返回 `false`。调用方（main.cpp）接收到 false 后输出提示信息，并自动回退到使用 `initDefaultData()` 初始化默认数据。

---

## 九、批量导入（batch_input.h / batch_input.cpp）

### 9.1 支持格式

每行一条记录，`|` 分隔字段，`#` 或 `;` 开头为注释行：

| 类型 | 格式 | 示例 |
|------|------|------|
| DEPT | 名称\|描述 | `DEPT\|皮肤科\|诊治皮肤疾病` |
| DOCTOR | 姓名\|级别\|科室ID\|出诊日 | `DOCTOR\|张医生\|0\|DEPT001\|1,3,5` |
| PATIENT | 姓名\|年龄\|联系方式\|类型 | `PATIENT\|张三\|30\|1380000\|0` |
| MEDICINE | 商品名\|通用名\|别名\|规格\|库存 | `MEDICINE\|阿莫西林\|阿莫西林\|\|0.25g*24\|500` |
| WARD | 类型\|科室ID\|床位数 | `WARD\|0\|DEPT001\|30` |
| ADMIN | 管理员ID\|密码 | `ADMIN\|user001\|pass123` |
| REG | 患者ID\|科室ID\|医生ID | `REG\|PAT001\|DEPT001\|DOC001` |
| DEPTMED | 科室ID\|药品ID | `DEPTMED\|DEPT001\|MED001` |

### 9.2 核心函数

**`batchImportFromFile(const char* filename)`**：打开文件，逐行读取，跳过空行和注释行。每行调用 `parseLine()` 处理。返回成功导入的记录数。

**`batchImportFromStdin()`**：从标准输入逐行读取，遇到空行结束。

**`parseLine(const string& line)`**：核心解析函数。使用 `split(line, '|')` 分割字段，根据第一字段 `type` 判断实体类型，调用对应的 `generateXxxID()` 生成ID，`new` 创建节点，`insertXxxTail()` 插入链表。对于 ADMIN 类型额外检查重复ID（`findAdminByID`），对于 DEPTMED 类型额外检查重复关联（`findDeptMedicine`）。

**`parseWorkDays(const string& str, int* days)`**：将逗号分隔的字符串（如 "1,3,5"）解析为整数数组，带重复检测。

### 9.3 集成方式

主菜单中增加了选项"4. 批量数据导入"，可选择从文件导入（输入文件名）或从键盘标准输入。batch_input.cpp 中的函数直接操作链表头指针，导入的数据实时生效。

---

## 十、管理员功能（menus_admin.h / menus_admin.cpp）

约1850行代码，24个函数，实现10项管理功能 + 登录。

### 10.1 adminLogin()——管理员登录

```cpp
bool adminLogin(AdminNode* adminHead) {
    int attempts = 0;
    while (attempts < 3) {
        printTitle("管理员登录");
        string adminID = readString("请输入管理员ID (0=取消): ", 10);
        if (adminID == "0") return false;

        string password;
        cout << "请输入密码: ";
        getline(cin, password);  // 直接读取密码（readString之前已处理缓冲）

        AdminNode* admin = findAdminByID(adminHead, adminID);
        if (admin != nullptr && admin->password == password) {
            cout << "\n登录成功! 欢迎, " << adminID << "!" << endl;
            pauseScreen();
            return true;
        } else {
            attempts++;
            cout << "登录失败! 剩余尝试次数: " << (3 - attempts) << endl;
        }
    }
    return false;
}
```

读密码时使用 `getline(cin, password)` 直接读取——因为 `readString`（读取ID时调用）内部使用 `getline` 并正确消费了换行符，所以无需额外的 `clearInputBuffer()` 操作。

### 10.2 科室管理（7个函数）

**`adminDeptAdd`**：`readString` 输入名称 → `readOptionalString` 输入描述 → `generateDeptID()` 自动生成ID → 头插法插入。

**`adminDeptModify`**：`findDepartmentByID` 查找 → 显示当前值 → 用 `readOptionalString` 读取新值（空串 = 不修改）。

**`adminDeptDelete`**：查找 → **外键检查**：遍历 DoctorNode 统计关联医生数，遍历 WardNode 统计关联病房数。如果有任何关联则拒绝删除并提示。这是保证数据完整性的关键设计。

**`adminDeptAssignWard`**：列出所有病房 → 选择病房 → 列出所有科室 → 选择目标科室 → 修改 `ward->departmentID`。

**`adminDeptAssignMedicine`**：列出所有药品 → 输入药品ID和科室ID → 调用 `isMedicineInDept` 防重复 → 创建 `DeptMedicineNode` 并插入链表。

### 10.3 医生管理（4个函数）

**`adminDoctorAdd`**：
1. `readString` 输入姓名（可取消）
2. `readIntRange(-1, 3)` 选择级别（-1取消）
3. 列出科室供选择 → `readString` 输入科室ID → `deptExists` 验证
4. `readString` 输入出诊日（逗号分隔1-7）
5. **出诊日解析**：`stringstream ss(workDaysStr)` → `while(getline(ss, token, ','))` → 去除每项首尾空白 → `atoi(token.c_str())` 转为整数 → 校验范围1-7 → 去重 → 存入 `workDays[]`
6. `generateDoctorID()` 生成ID → 头插法插入

**`adminDoctorModify`**：
- `readIntRange(-2, 3)` 选择级别。**关键**：cancel 值使用 `-2` 而非 `0`，因为 `0` 是合法的医生级别（主任医师）。
- 其他字段通过 `readOptionalString` 逐项修改

**`adminDoctorDelete`**：查找 → 遍历 RegistrationNode 链表统计该医生的**所有挂号记录**（不限于待就诊状态）→ 给出警告 → 确认后物理删除

### 10.4 患者管理（4个函数）

**`adminPatientAdd`**：姓名 → `readIntRange(-1, 150)` 年龄 → 联系方式 → `readIntRange(-1, 1)` 类型（0门诊/1住院）→ 所有验证通过后才调用 `generatePatientID()`。

**`adminPatientDelete`**：检查活跃住院记录 → 有则拒绝删除 → 否则物理删除。

### 10.5 挂号管理（2个函数）

**`adminRegCreate`**——最复杂的业务逻辑：

1. **选择患者**：输入患者ID，`findPatientByID` 验证
2. **选择科室**：输入科室ID，`findDepartmentByID` 验证
3. **筛选当日出诊医生**：
   ```cpp
   for (DoctorNode* d = doctorHead; d != nullptr; d = d->next) {
       if (d->departmentID == deptID &&  // 属于所选科室
           doctorWorksOnDay(d, (int)week)) { // 今天出诊
           // 显示为可选医生
       }
   }
   ```
4. **防重复挂号**：遍历 RegistrationNode，查找是否已有 `patientID + deptID + regDay + STATUS_PENDING` 同时匹配的记录
5. `generateRegID(day)` 生成挂号ID → 尾插法插入

### 10.6 住院管理（3个函数）

**`adminHospAdmit`——入院流程**：
1. 输入门诊患者ID → 验证类型必须是门诊
2. 从挂号记录获取患者科室 → 筛选同科室病房
3. 显示可用床位（遍历 `ward->bedStatus[]`，显示空闲/占用状态）
4. 用户选择床位号（1索引）→ 验证空闲 → 转换为0索引
5. 选择主管医生（最多10名，遍历 DoctorNode，多选，防重复）
6. 输入押金 → 修改患者类型为住院 → 直接写入 `ward->bedStatus[bedIdx] = patientID`
7. 创建 HospitalizationNode（`generateHospID(day)`）

**`adminHospDischarge`——出院结算**：
1. 查找在院记录
2. 显示住院天数（`day - admitDay`）
3. **结算**：由于每日费用已在时间推进中从押金扣除，出院时直接使用当前押金余额作为退款金额。不需要再次加减 money。
4. `releaseBed(ward, bedNo)` 释放床位 → 患者类型恢复门诊

### 10.7 药房管理（3个函数）

**`adminPharmacyStockIn`**：选择药品 → `readInt` 输入数量 → `stock += qty`

**`adminPharmacyStockOut`**：选择药品 → `readInt` 输入数量 → **校验库存充足**（`qty > med->stock` 拒绝）→ `stock -= qty` → `consumed += qty`

### 10.8 药品管理（4个函数）

**`adminMedAdd`**：商品名 → 通用名 → 别名 → 规格 → 库存 → 所有验证通过后 `generateMedID()`

**`adminMedDelete`**：确认后物理删除

### 10.9 时间推进（adminTimeAdvance）——核心自动化机制

```cpp
static void adminTimeAdvance(PatientNode* patientHead,
                             RegistrationNode* regHead,
                             HospitalizationNode* hospHead) {
    printTitle("时间推进");
    int seconds = readIntRange("请输入推进秒数 (>0, 0=取消): ", 0, 99999999);
    if (seconds == 0) return;

    unsigned int daysAdvanced = (unsigned int)(seconds / SECONDS_PER_DAY);  // 向下取整
    unsigned int remainderSec = (unsigned int)(seconds % SECONDS_PER_DAY);

    // 逐日循环处理（每完整一天扣一次费）
    for (unsigned int d = 0; d < daysAdvanced; d++) {
        setTime(globalTime + SECONDS_PER_DAY);  // 推进一天

        // 扣住院费
        for (HospitalizationNode* h = hospHead; h != nullptr; h = h->next) {
            if (h->status != 0) continue;  // 跳过非在院
            h->deposit = round2(h->deposit - HOSPITAL_DAILY_FEE);
            money = round2(money + HOSPITAL_DAILY_FEE);
            // 同步患者记录
            PatientNode* p = findPatientByID(patientHead, h->patientID);
            if (p) { p->deposit = h->deposit; p->admitDays++; }
        }

        // 过期挂号
        for (RegistrationNode* r = regHead; r != nullptr; r = r->next) {
            if (r->status == STATUS_PENDING && r->regDay < day) {
                r->status = STATUS_EXPIRED;
                expiredCount++;
            }
        }
    }

    // 剩余秒数：仅推进时间，不计费
    if (remainderSec > 0) setTime(globalTime + remainderSec);
    // 显示汇总信息...
}
```

**关键设计**：(1) 向下取整确保不足一天不计费 (2) 住院费先扣后同步到 PatientNode (3) 挂号过期检查使用 `< day` 而非 `<=`，确保当天挂号不过期 (4) 剩余秒数不计费。

### 10.10 adminMenu()——主循环

```cpp
bool adminMenu(DepartmentNode*& deptHead, ...) {
    while (true) {
        printTitle("管理员主菜单");
        printStatus();
        int choice = readIntRange("请选择(0-11): ", 0, 11);
        switch (choice) {
            case 1: adminDeptMenu(...); break;
            case 2: adminWardMenu(...); break;
            case 3: adminDoctorMenu(...); break;
            case 4: adminMedMenu(...); break;
            case 5: adminPatientMenu(...); break;
            case 6: adminRegMenu(...); break;
            case 7: adminHospMenu(...); break;
            case 8: adminPharmacyMenu(...); break;
            case 9: adminReportMenu(...); break;
            case 10: adminTimeAdvance(...); break;
            case 11: saveAllData(...); break;
            case 0: return false;  // 退出登录
        }
    }
}
```

---

## 十一、医生功能（menus_doctor.h / menus_doctor.cpp）

约975行，实现7项功能。

### 11.1 doctorMenu()——入口函数

```cpp
bool doctorMenu(DoctorNode*& doctorHead, PatientNode* patientHead, ...) {
    // 1. 登录
    string doctorID = readString("请输入医生ID (输入0退出): ", 20);
    DoctorNode* doctor = findDoctorByID(doctorHead, doctorID);
    if (!doctor) { cout << "[错误] 医生ID不存在！" << endl; return false; }

    // 2. 检查当日是否为该医生工作日
    if (!doctorWorksOnDay(doctor, (int)week)) {
        cout << "[错误] 今天(" << WEEKDAY_NAMES[week - 1]
             << ")不是您的工作日，不能登录！" << endl;
        // 列出该医生的所有工作日
        return false;
    }

    // 3. 主循环
    while (loggedIn) {
        cout << "当前医生: " << doctor->name << " (" << doctor->doctorID << ")" << endl;
        int choice = readIntRange("请选择功能(0-7): ", 0, 7);
        switch (choice) {
            case 1: 查看待诊患者 break;
            case 2: 接诊 break;
            case 3: 开具处方 break;
            case 4: 开具检查 break;
            case 5: 查看住院患者 break;
            case 6: 为住院患者诊治 break;
            case 7: 医生报表 break;
            case 0: return true;
        }
    }
}
```

### 11.2 showWaitingPatients()——查看待诊患者

遍历 RegistrationNode 链表，筛选条件三个：
```cpp
cur->doctorID == doctor->doctorID        // 挂号为本人
&& cur->status == STATUS_PENDING         // 状态为待就诊
&& cur->regDay == day                // 仅当日挂号
```

每匹配一条，用 `findPatientByID` 解析患者姓名并显示。

### 11.3 menuConsultPatient()——接诊

1. 调用 `showWaitingPatients` 列出待诊患者
2. 输入挂号ID → `findRegistrationByID` 验证 → 检查归属（是否挂号为本人）→ 检查状态（是否为待就诊）
3. 输入主诉和诊断（`readString`，最大100字符）
4. 创建 ConsultationNode → `generateConsultID()` → 关联挂号ID → 设置 consultDay = day
5. 更新挂号状态为 `STATUS_SEEN`
6. 输出就诊ID供后续开处方/检查使用

### 11.4 menuPrescribe()——开具处方

1. 输入就诊ID → 验证归属和状态
2. **展示科室药品**：`showDeptMedicines()` 通过双重遍历实现：
   - 外层：遍历 MedicineNode（所有药品）
   - 内层：遍历 DeptMedicineNode（检查药品是否关联到医生科室）
   - 显示匹配药品的ID、商品名、库存
3. **循环添加药品**：
   - 输入药品ID → `findMedicineByID` 验证
   - 检查科室归属（`isMedInDoctorDept`）
   - 输入数量 → 库存不足则警告但允许继续（医生可能给库存不足的药品开处方，但缴费时才扣库存）
   - 输入单价（默认15元）
   - 创建 PrescMedicineNode → 插入链表
4. 创建 PrescriptionNode → `generatePrescID()` → 总金额 = sum(数量×单价)
5. 取消时清理已创建的 PrescMedicineNode（遍历删除防止内存泄漏）

### 11.5 menuOrderExamination()——开具检查

输入就诊ID → 输入检查项目名称 → 输入费用 → 创建 ExaminationNode

### 11.6 menuViewInpatients()——查看住院患者

遍历 HospitalizationNode，检查本医生是否在 `doctorIDs[]` 数组中。展示患者姓名（通过 `findPatientByID` 解析）、病房、床位、押金。可选择查看某患者的完整诊疗记录（调用 showPatientConsultations、showPatientPrescriptions、showPatientExaminations）。

### 11.7 menuPrescribeForInpatients()——为住院患者诊治

与门诊开处方/检查流程相同，但 consultID 可选（住院处方不强制关联看诊记录）。

---

## 十二、患者功能（menus_patient.h / menus_patient.cpp）

约545行，3项功能。

### 12.1 patientMenu()——入口函数

输入患者ID → `findPatientByID` 验证 → 自动识别门诊/住院类型 → 显示对应菜单。

### 12.2 viewMedicalRecords()——多链表关联查询

依次遍历5条链表（RegistrationNode、ConsultationNode、ExaminationNode、PrescriptionNode、HospitalizationNode），筛选匹配患者ID的记录。

**处方药品明细的获取**：
1. 遍历 PrescriptionNode：找到 match 患者ID的处方
2. 遍历 PrescMedicineNode：找到 `prescID` 匹配的关联项
3. 调用 `findMedicineByID`：从 medHead 中获取药品名称
4. 显示："药品名 x数量"

这是典型的**三层链表关联查询**：处方 → 处方药品关联 → 药品信息。

### 12.3 payFees()——支付费用

这是系统的核心财务功能：

1. **收集待缴费项目**：
   - 遍历 RegistrationNode：status == STATUS_PENDING → 挂号费 = 20元/笔
   - 遍历 PrescriptionNode：status == 0（未缴费）→ 处方金额

2. **支付选项**：全部 / 仅挂号 / 指定处方

3. **处理支付**：
   - 门诊患者：`money = round2(money + amount)` ——医院直接收款
   - 住院患者：`hosp->deposit -= amount; money += amount` ——从押金扣除并转医院收入

4. **状态更新**：
   - 所有已支付的挂号 → `r->status = STATUS_SEEN`
   - 所有已支付的处方 → `pr->status = 2`（已缴费）

5. **库存扣减**：遍历 PrescMedicineNode → 对每个药品：`med->stock -= pm->quantity; med->consumed += pm->quantity`。如果库存变负则截断为0（安全保护）。

### 12.4 viewHospitalizationInfo()——住院信息

调用 `findActiveHospitalizationByPatient` 获取活跃住院记录 → 显示病房、床位（+1显示）、主管医生、天数、押金。计算累计费用=天数×200。如果押金不足则提示警告。

---

## 十三、报表查询（reports.h / reports.cpp）

约1050行，实现三个入口函数 + 10个具体报表函数。

### 13.1 管理员报表——adminReportMenu()

7种报表通过菜单选择：

**药品查询（reportDrugQuery）**：
- 输入科室ID或"all"查看全部
- 数据收集：遍历 MedicineNode → 用 `findDeptMedicine` 筛选 → 存入 `std::vector<MedDisplayInfo>`
- 排序选项：按库存升/降序、消耗量升/降序、名称字典序（使用 `std::sort` + lambda 表达式）
- 搜索过滤：大小写不敏感的子串匹配（`std::tolower` 逐字符转换后比较）

**医生信息（reportDoctorInfo）**：
- 按科室筛选 → 存入 vector<DoctorNode*> → 按 doctorID 排序
- 显示级别、科室名、出诊日（遍历 workDays[] 用 WEEKDAY_NAMES 转换）

**病房床位（reportWardBeds）**：
- 按科室筛选病房 → 遍历 bedStatus[] → 每床显示"空闲"或患者名（调用 findPatientByID 解析）

**患者就诊记录（reportPatientRecords）**：
- 调用 `displayPatientRecords` 单/全患者
- 该函数遍历5条链表聚合：挂号（显示科室名、医生名、状态）→ 看诊（显示主诉、诊断）→ 检查 → 处方（含药品明细，再次遍历 PrescMedicineNode）→ 住院

**资金统计（reportFundStats）**：
- 挂号费：遍历 RegistrationNode × 20
- 药费：遍历 PrescriptionNode（status==2 已支付）
- 住院费：遍历 HospitalizationNode × (endDay - admitDay) × 200

**医生工作量（reportDoctorWorkload）**：
- 指定时间范围 → 遍历 DoctorNode → 对每个医生遍历 ConsultationNode 和 PrescriptionNode 计数
- 存入 vector<DoctorWorkload> → 按总量降序排列 → 显示排名

**营业总额（reportTotalRevenue）**：
- 指定时间范围 → 挂号费（范围内 registrations × 20）+ 药费（范围内已支付处方总额）+ 住院费（住院天数与范围取交集 × 200）
- 住院天数交集算法：`overlapStart = max(admitDay, startDay); overlapEnd = min(endDay_actual, endDay); days = overlapEnd - overlapStart`

### 13.2 医生报表——doctorReportMenu()

**门诊患者（doctorReportOutpatients）**：指定时间范围 → 遍历 RegistrationNode 筛选本医生的挂号 → 对每条挂号查找关联的 ConsultationNode 显示诊疗详情。

**住院患者（doctorReportInpatients）**：遍历 HospitalizationNode → 检查 `doctorIDs[]` 数组中是否包含本人 → 显示患者信息、病房床位、押金。

**处方统计（doctorReportPrescriptions）**：遍历 PrescriptionNode 筛选本医生的处方 → 按已支付/未支付/已取消分组统计 → 显示每张处方的药品明细（遍历 PrescMedicineNode）。

### 13.3 患者报表——patientReportMenu()

显示就诊摘要（统计各类型记录次数）→ 完整记录查看 → 当前住院信息。

---

## 十四、主程序（main.cpp）

### 14.1 全局链表头指针

```cpp
DepartmentNode* deptHead = nullptr;    DoctorNode* doctorHead = nullptr;
PatientNode* patientHead = nullptr;     RegistrationNode* regHead = nullptr;
ConsultationNode* consultHead = nullptr; ExaminationNode* examHead = nullptr;
PrescriptionNode* prescHead = nullptr;  MedicineNode* medHead = nullptr;
PrescMedicineNode* prescMedHead = nullptr; DeptMedicineNode* deptMedHead = nullptr;
WardNode* wardHead = nullptr;           HospitalizationNode* hospHead = nullptr;
AdminNode* adminHead = nullptr;
```

13个头指针，初始为 nullptr。这些指针作为全局变量定义在 main.cpp 中，其他模块通过 `extern` 关键字引用。

### 14.2 启动流程

```cpp
int main() {
    // 1. 尝试加载存档
    if (!loadAllData(deptHead, doctorHead, ..., adminHead)) {
        // 2. 加载失败 → 默认初始化
        setTime(0);          // globalTime=0, day=1
        money = 10000.00;    // 初始资金
        initDefaultData(deptHead, doctorHead, ..., adminHead);
    }
    // 3. 显示统计
    cout << "当前科室数: " << countDepartments(deptHead) << endl;
    // 4. 进入主循环
    mainMenu();
    // 5. 清理内存
    cleanupAll();  // 遍历所有链表 free
    return 0;
}
```

### 14.3 mainMenu()——主菜单循环

```cpp
void mainMenu() {
    while (true) {
        printTitle("小型医院医疗管理系统");
        printStatus();
        int choice = readIntRange("请选择(0-4): ", 0, 4);
        switch (choice) {
            case 1: 管理员登录 → adminMenu 循环
            case 2: 医生登录 → doctorMenu
            case 3: 患者登录 → patientMenu
            case 4: 批量导入 → batchImportFromFile 或 batchImportFromStdin
            case 0: 保存数据 → saveAllData → 退出
        }
    }
}
```

### 14.4 cleanupAll()——内存清理

退出前调用，遍历13条链表头指针，逐一调用对应的 `freeXxxList()` 释放所有动态分配的节点内存。这是防止内存泄漏的最后保证。

---

## 十五、健壮性设计详解

### 15.1 输入校验层

系统所有用户输入都通过6个读取函数（readInt、readDouble、readString、readOptionalString、readIntRange、readNonNegDouble）处理，形成一道**统一的输入防线**。每个函数都实现了：
- **类型错误处理**：cin failbit 检测与恢复（`cin.clear()` + `cin.ignore()`）
- **范围校验**：readIntRange 限定合法区间
- **空值拒绝**：readString 禁止空输入（循环重试）
- **长度限制**：readString 检查输入长度不超过最大值
- **循环重试**：所有错误都回到 `while(true)` 循环开始，重新提示

### 15.2 空指针防护

所有 `findXxxByID` 的返回值在使用前都经过 null 检查。例如：
```cpp
DoctorNode* doc = findDoctorByID(doctorHead, id);
if (!doc) {
    cout << "[错误] 医生ID不存在！" << endl;
    return;
}
// 安全使用 doc->name, doc->level 等
```

### 15.3 逻辑冲突处理

| 场景 | 防御机制 | 实现位置 |
|------|----------|----------|
| 重复挂号（同患者+同科室+同日） | 遍历 RegistrationNode 四条件匹配检查 | adminRegCreate |
| 分配已占用床位 | 检查 `bedStatus[bedIdx]` 非空则拒绝 | adminHospAdmit |
| 医生不在当天出诊 | `doctorWorksOnDay` 过滤，挂号时只列出出诊医生 | adminRegCreate |
| 给非关联科室的医生开药 | 开处方时调用 `isMedInDoctorDept` 验证 | menuPrescribe |
| 删除有医生的科室 | 遍历 DoctorNode 统计关联数>0则拒绝 | adminDeptDelete |
| 删除有占用床位的病房 | 遍历 `bedStatus[]`，有非空则拒绝 | adminWardDelete |
| 住院不足一天出院 | `day - admitDay` 可能为0，取 `max(1, result)` | adminHospDischarge |
| 库存不足出库 | `qty > med->stock` 则拒绝 | adminPharmacyStockOut |
| 押金不足继续住院 | 允许扣为负数，提示"押金不足" | adminTimeAdvance |
| 删除有活跃住院的患者 | `findActiveHospitalizationByPatient` 检查 | adminPatientDelete |

### 15.4 文件异常处理

| 异常 | 处理 |
|------|------|
| 文件不存在 | `ifstream` 打开失败 → 提示 → 返回 false → 调用方回退到默认数据 |
| 格式错误 | 字段数量不足 → 关闭文件 → 返回 false |
| 内容不完整 | 部分解析成功后续失败 → 整体返回 false（不保留部分数据） |

### 15.5 内存安全

- 所有 `new` 分配都有对应的释放路径
- 程序退出时 `cleanupAll()` 遍历13条链表逐一 `delete`
- 处方取消时清理孤立的 PrescMedicineNode 节点
- 无悬空指针（所有释放后将指针置空或由循环跳过）

---

## 十六、关键算法实现细节

### 16.1 链表遍历与筛选

```cpp
NodeType* cur = head;           // 从头节点开始
while (cur != nullptr) {        // 遍历到尾节点(NULL)
    if (cur->field == target) { // 条件匹配
        // 处理当前节点
    }
    cur = cur->next;            // 前进到下一个节点
}
```

所有查询操作都遵循此模式。对于多条件筛选，使用 `&&` 组合条件。

### 16.2 链表物理删除

```cpp
// 情况1：删除头节点
if (head->key == target) {
    NodeType* tmp = head;
    head = head->next;  // 头指针后移
    delete tmp;          // 释放内存
    return true;
}
// 情况2：删除中间/尾节点
NodeType* cur = head;
while (cur->next != nullptr && cur->next->key != target)
    cur = cur->next;    // 找到前驱节点
if (cur->next == nullptr) return false;  // 未找到
NodeType* tmp = cur->next;
cur->next = tmp->next; // 跳过被删节点
delete tmp;             // 释放内存
```

### 16.3 链表插入（三种方式）

```cpp
// 头插 O(1)
node->next = head;
head = node;

// 尾插 O(n)
if (!head) { head = node; return; }
NodeType* cur = head;
while (cur->next) cur = cur->next;
cur->next = node;

// 字典序插入 O(n)
if (!head || head->key > node->key) { /* 插头部 */ return; }
NodeType* cur = head;
while (cur->next && cur->next->key < node->key) cur = cur->next;
node->next = cur->next;
cur->next = node;
```

### 16.4 多链表关联查询

以"查看患者处方药品"为例的三层查询：
```
Level 1: prescHead → 找到患者的所有处方 (match patientID)
Level 2: prescMedHead → 找到每张处方的药品项 (match prescID)
Level 3: medHead → 找到药品的名称信息 (match medID via findMedicineByID)
```

### 16.5 时间推进循环

```
输入秒数 seconds
↓
daysAdvanced = seconds / 86400  (整数除法，向下取整)
remainder = seconds % 86400
↓
循环 daysAdvanced 次:
  ├── setTime(globalTime + 86400)  →  day++
  ├── 遍历住院链表: deposit -= 200, money += 200
  ├── 同步 PatientNode: p->deposit = h->deposit, p->admitDays++
  └── 遍历挂号链表: regDay < day → STATUS_EXPIRED
↓
if (remainder > 0): setTime(globalTime + remainder)  [不计费]
```

### 16.6 数据持久化流程

**保存**：
```
每个链表 → while(cur) → esc(字段) → fout << 字段1|字段2|... → fout.close()
```

**加载**：
```
打开文件 → while(getline(line)) → splitLine(line, '|') → 创建节点 → insertTail
                                                                         ↓
                                                            updateIDCountersFromLists()
                                                            (同步10个全局计数器)
```

---

## 十七、系统亮点总结

| 序号 | 亮点 | 说明 |
|------|------|------|
| 1 | 纯链表实现 | 12类实体全部使用手动管理的单向链表，不依赖STL容器 |
| 2 | ID关联解耦 | 实体间通过string ID关联，避免指针复杂性 |
| 3 | 统一时钟 | globalTime唯一入口setTime()，day和week自动推导 |
| 4 | 三角色分离 | 管理员11项、医生7项、患者3项，功能视图不同 |
| 5 | 完整医疗流程 | 挂号→接诊→处方→检查→缴费→住院→出院 |
| 6 | 自动化机制 | 时间推进自动扣费、过期挂号、批量导入 |
| 7 | 金融准确性 | 所有金额经round2()处理，防浮点误差 |
| 8 | 崩不溃 | readInt等6个输入函数保证任何输入不崩溃 |
| 9 | 数据持久化 | 14个文件，转义/反转义，异常回退 |
| 10 | 批量导入 | 支持8种实体、管道分隔格式、文件/标准输入双模式 |
| 11 | 多维度报表 | 管理员7种+医生3种+患者个人 |
| 12 | 内存安全 | 所有new有对应的delete，退出时完整清理 |
