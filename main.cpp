#include <iostream>
#include <vector>
#include <Windows.h>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <random>
using namespace std;
#define unknown 100
//储存公式的类
class Formula {
public:
    // 此处的文字一共有两种形式， 1代表取正，0代表取反
    vector<int> frequence_word; // 文字出现的频率
    vector<vector<int>> sentences;// 子句,二维数组存储
    vector<bool> sign; // 一个文字最终是正还是负数
    Formula() {
        frequence_word.clear();
        sentences.clear();
        sign.clear();
    } // 默认构造函数
    Formula(const Formula& f) { // 拷贝构造函数，等会儿要用到
        sentences = f.sentences;
        frequence_word = f.frequence_word;
        sign = f.sign;
    }
};

/*
 实现功能的类
 */
class DPLL {
public:
    Formula formula;               // 初始公式
    int num_word;             // 公式中的变量数量
    vector<vector<int>> data; // 存储测试数据
    int num_sentence;              // 公式中的子句数量
    int single_sentence_rule(Formula&);  // 单子句规则
    DPLL() {}
    int work(Formula&);             // 递归计算DPLL
    int detele_word(Formula&, int need_to_delete); // 单子句规则
    void input();
    void output();

};


void DPLL::input() {
    cout << "please put in the number of words and the number of sentenses" << endl;
    cin >> num_word;
    cin >> num_sentence;
    // 开始让文件读入

    data.resize(num_sentence);
    for (int i = 0; i < num_sentence; i++) {
        data[i].clear();
    }

    // 第一步生成数据
    default_random_engine e;
    uniform_int_distribution<int> u(-num_word, num_word);//注意，这里的范围是闭区间
    // 应当除去随机数中的0
    for (int i = 0; i < num_sentence; i++) {
        // 确定析取子式的长度
        int x = u(e);
        while (x <= 0) x = u(e);
        for (int j = 0; j < x; j++) {
            // 确定一个不为0的数字
            int y = u(e);
            while (y == 0)y = u(e);
            data[i].push_back(y);
        }
    }

    // 初始化数据
    formula.sentences.resize(num_sentence);
    formula.frequence_word.resize(num_word, 0);
    formula.sign.resize(num_word, 0);

    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data[i].size(); j++) {
            if (data[i][j] > 0) {
                formula.sentences[i].push_back(data[i][j]);// 压入栈中
                formula.frequence_word[data[i][j] - 1]++;// 出现频率自增
            }
            else if (data[i][j] < 0) {
                formula.sentences[i].push_back(data[i][j]);
                formula.frequence_word[-data[i][j] - 1]++;
            }
        }
    }
}

// 这里是利用单子句规则化简
int DPLL::single_sentence_rule(Formula& f) {
    if (f.sentences.size() == 0) // 特判，句子为空必真
    {
        return true;
    }
    bool if_find_word = true; // 是否已经发现了一个文字
    while (if_find_word) {  // 能找到文字就接着找，找不到就停止
        if_find_word = false; // 默认找不到
        for (int i = 0; i < f.sentences.size(); i++) {
            if (f.sentences[i].size() == 1) // 如果是文字
            {
                if_find_word = true;//已经发现了一个文字，开始化简
                f.frequence_word[abs(f.sentences[i][0]) - 1] = 0; // 将该文字全部删除，故频率为0
                f.sign[abs(abs(f.sentences[i][0]) - 1)] = true; // 该文字需要使用
                int result = detele_word(f, abs(f.sentences[i][0]));
                //化简以后如果空了，那就是真的
                //化简以后不空，但出现了空子句，那就是假的
                if (result == true || result == false) {
                    return result;
                }// 出结果了！
            }
            else if (f.sentences[i].size() == 0) //出现空子句，说明不可满足
            {
                return false; // 该公式不可满足
            }
        }
    }
    // 到此为止判断不了，说明已经删除光了所有文字
    return unknown;
}

// 单子句规则中的删除操作
int DPLL::detele_word(Formula& f, int need_to_delete) {
    for (int i = 0; i < f.sentences.size(); i++) {
        for (int j = 0; j < f.sentences[i].size(); j++) {
            //两种可能的情况需要删除：1.含有该文字的析取子式；2.含有该文字的否定得到析取子式
            if (need_to_delete == f.sentences[i][j]) { // 含有该文字
                f.sentences.erase(f.sentences.begin() + i); // 移除了i指向的析取子式
                i--;                // f.sentence.size()总数减一，因此为了完全遍历，i也减一
                if (f.sentences.size() == 0) //移除操作可能使得该句子成为空子句，此时该公式可满足
                {
                    return true;
                }
                break;
            }
            else if (-need_to_delete == f.sentences[i][j]) // 含有该文字的否定
            {
                f.sentences[i].erase(f.sentences[i].begin() + j); // 将该文字的否定从这个析取子式中移除
                j--;    // 删除了该否定以后再检查一遍看看能不能再删除，一直删到不再有这种这个文字的否定为止
                if (f.sentences[i].size() == 0) // 公式空了，那就不可满足
                {
                    return false;
                }
            }
        }
    }
    // 此时用我们的办法无法判断出是对是错，故认为不成立
    return unknown;
}

// 分裂规则
int DPLL::work(Formula& f) {
    int result = single_sentence_rule(f); // 先用单子句规则进行判断
    if (result == true || result == false) // 已经满足，再也用不着判断
    {
        return result;
    }
    // 这里出现最多的对应ppt中的L
    // 下面这一行代码，确定L这个文字是谁
    int L = distance(f.frequence_word.begin(), max_element(f.frequence_word.begin(), f.frequence_word.end())) + 1;
    // 分裂规则
    Formula s1 = f, s2 = f;
    //先处理s1

    //printf("this is s1    origin\n");
    //printf("%d the biggest\nthis is frequence\n\n", L);
    //for (int i = 0; i < s1.frequence_word.size(); i++) {
    //    printf("%d ", s1.frequence_word[i]);
    //}
    //printf("\nthis is sentences\n");
    //for (int i = 0; i < s1.sentences.size(); i++) {
    //    for (int j = 0; j < s1.sentences[i].size(); j++) {
    //        printf("%d ", s1.sentences[i][j]);
    //    }
    //    printf("\n");
    //}
    //printf("over\n");

    for (int i = 0; i < s1.sentences.size(); i++) {
        for (int j = 0; j < s1.sentences[i].size(); j++) {
            if (s1.sentences[i][j] == L) {
                s1.frequence_word[L - 1]--;
                for (int k = j + 1; k < s1.sentences[i].size(); k++) {
                    if (s1.sentences[i][k] == -L)return false;
                }// 如果一个句子中出现了L，那就绝对不能出现-L，否则错！！！
                s1.sentences[i].erase(s1.sentences[i].begin() + j);
                j--;
            }
            else if (s1.sentences[i][j] == -L) {
                for (int k = 0; k < s1.sentences[i].size(); k++) {
                    s1.frequence_word[abs(s1.sentences[i][k]) - 1]--;
                }//注意！！！在该句中出现的每个文字，其频率都要自减！！！不能只减去L的频率！！！
                s1.sentences.erase(s1.sentences.begin() + i);
                i--;//i 回退！
                break;
            }
        }
    }

    //后处理s2
    for (int i = 0; i < s2.sentences.size(); i++) {
        for (int j = 0; j < s2.sentences[i].size(); j++) {
            if (s2.sentences[i][j] == -L) {
                s2.frequence_word[L - 1]--;
                for (int k = j + 1; k < s1.sentences[i].size(); k++) {
                    if (s1.sentences[i][k] == L)return false;
                }// 如果一个句子中出现了-L，那就绝对不能出现L，否则错！！！
                s2.sentences[i].erase(s2.sentences[i].begin() + j);
                j--;
            }
            else if (s2.sentences[i][j] == L) {
                for (int k = 0; k < s2.sentences[i].size(); k++) {
                    s2.frequence_word[L - 1]--;
                }//注意！！！在该句中出现的每个文字，其频率都要自减！！！不能只减去L的频率！！！
                s2.sentences.erase(s2.sentences.begin() + i);
                i--;
                break;
            }
        }
    }

    int result1 = work(s1);
    for (int i = 0; i < s1.sign.size(); i++) {
        if (s1.sign[i] == true)f.sign[i] = true;
    }
    if (result1 == true)return true;
    
    int result2 = work(s2);
    /*printf("this is s2\n");
    for (int i = 0; i < s2.sentences.size(); i++) {
        for (int j = 0; j < s2.sentences[i].size(); j++) {
            printf("%d ", s2.sentences[i][j]);
        }
        printf("\n");
    }
    printf("over\n");*/
    for (int i = 0; i < s2.sign.size(); i++) {
        if (s2.sign[i] == true)f.sign[i] = true;
    }
    if (result2 == true)return true;
    return false;
}

//调用上述函数
void DPLL::output() {
    int result = work(formula);
    if (result == true) // 公式可满足
    {
        cout << "SAT" << endl;
        cout << "please check the solution in the result.txt" << endl;
    }
    else // 公式不可满足
    {
        cout << "UNSAT" << endl;
        cout << "please check the counter-example in the result.txt" << endl;
    }
    fstream file1, file2;
    file1.open("data.txt", ios::out);
    file2.open("result.txt", ios::out);

    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data[i].size(); j++) {
            file1 << data[i][j] << " ";
        }
        file1 << endl;
    }
    file1.close();

    for (int i = 0; i < formula.sign.size(); i++) {
        file2 << i + 1;
        if (formula.sign[i])file2 << " T" << endl;
        else file2 << " F" << endl;
    }
    file2.close();

}

int main() {
    DPLL a;
    DWORD begin_time = GetTickCount();
    a.input();
    a.output();
    DWORD end_time = GetTickCount();
    cout << endl << "总用时" << end_time - begin_time << "ms" << endl;
    return 0;
}