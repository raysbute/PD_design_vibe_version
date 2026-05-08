================================================
  批量导入测试文件说明
================================================

使用方法:
  1. 运行 hospital.exe
  2. 主菜单选择 "4. 批量数据导入"
  3. 选择 "1. 从文件" 导入
  4. 输入文件名，如: test\01_departments.txt

导入顺序建议（因为后面文件可能引用前面文件创建的ID）:
  1. 06_admins.txt          (管理员 - 无依赖)
  2. 01_departments.txt     (科室 - 无依赖)
  3. 04_medicines.txt       (药品 - 无依赖)
  4. 05_wards.txt           (病房 - 依赖科室)
  5. 02_doctors.txt         (医生 - 依赖科室)
  6. 03_patients.txt        (患者 - 无依赖)
  7. 07_dept_med_links.txt  (科室药品关联 - 依赖科室+药品)
  8. 08_registrations.txt   (挂号 - 依赖患者+科室+医生)

各文件说明:
  01_departments.txt       12行 - 新增12个科室
  02_doctors.txt           11行 - 新增11名医生
  03_patients.txt          25行 - 新增15名门诊+10名住院患者
  04_medicines.txt         16行 - 新增16种药品
  05_wards.txt              6行 - 新增6个病房
  06_admins.txt             5行 - 新增5个管理员
  07_dept_med_links.txt    19行 - 建立19条科室药品关联
  08_registrations.txt     11行 - 创建11条挂号记录
  09_mixed_sample.txt      20行 - 综合示例（所有类型混合）

================================================
