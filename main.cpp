#include <iostream>
#include <vector>
#include <Windows.h>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <random>
using namespace std;
#define unknown 100
//���湫ʽ����
class Formula {
public:
    // �˴�������һ����������ʽ�� 1����ȡ����0����ȡ��
    vector<int> frequence_word; // ���ֳ��ֵ�Ƶ��
    vector<vector<int>> sentences;// �Ӿ�,��ά����洢
    vector<bool> sign; // һ�����������������Ǹ���
    Formula() {
        frequence_word.clear();
        sentences.clear();
        sign.clear();
    } // Ĭ�Ϲ��캯��
    Formula(const Formula& f) { // �������캯�����Ȼ��Ҫ�õ�
        sentences = f.sentences;
        frequence_word = f.frequence_word;
        sign = f.sign;
    }
};

/*
 ʵ�ֹ��ܵ���
 */
class DPLL {
public:
    Formula formula;               // ��ʼ��ʽ
    int num_word;             // ��ʽ�еı�������
    vector<vector<int>> data; // �洢��������
    int num_sentence;              // ��ʽ�е��Ӿ�����
    int single_sentence_rule(Formula&);  // ���Ӿ����
    DPLL() {}
    int work(Formula&);             // �ݹ����DPLL
    int detele_word(Formula&, int need_to_delete); // ���Ӿ����
    void input();
    void output();

};


void DPLL::input() {
    cout << "please put in the number of words and the number of sentenses" << endl;
    cin >> num_word;
    cin >> num_sentence;
    // ��ʼ���ļ�����

    data.resize(num_sentence);
    for (int i = 0; i < num_sentence; i++) {
        data[i].clear();
    }

    // ��һ����������
    default_random_engine e;
    uniform_int_distribution<int> u(-num_word, num_word);//ע�⣬����ķ�Χ�Ǳ�����
    // Ӧ����ȥ������е�0
    for (int i = 0; i < num_sentence; i++) {
        // ȷ����ȡ��ʽ�ĳ���
        int x = u(e);
        while (x <= 0) x = u(e);
        for (int j = 0; j < x; j++) {
            // ȷ��һ����Ϊ0������
            int y = u(e);
            while (y == 0)y = u(e);
            data[i].push_back(y);
        }
    }

    // ��ʼ������
    formula.sentences.resize(num_sentence);
    formula.frequence_word.resize(num_word, 0);
    formula.sign.resize(num_word, 0);

    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data[i].size(); j++) {
            if (data[i][j] > 0) {
                formula.sentences[i].push_back(data[i][j]);// ѹ��ջ��
                formula.frequence_word[data[i][j] - 1]++;// ����Ƶ������
            }
            else if (data[i][j] < 0) {
                formula.sentences[i].push_back(data[i][j]);
                formula.frequence_word[-data[i][j] - 1]++;
            }
        }
    }
}

// ���������õ��Ӿ���򻯼�
int DPLL::single_sentence_rule(Formula& f) {
    if (f.sentences.size() == 0) // ���У�����Ϊ�ձ���
    {
        return true;
    }
    bool if_find_word = true; // �Ƿ��Ѿ�������һ������
    while (if_find_word) {  // ���ҵ����־ͽ����ң��Ҳ�����ֹͣ
        if_find_word = false; // Ĭ���Ҳ���
        for (int i = 0; i < f.sentences.size(); i++) {
            if (f.sentences[i].size() == 1) // ���������
            {
                if_find_word = true;//�Ѿ�������һ�����֣���ʼ����
                f.frequence_word[abs(f.sentences[i][0]) - 1] = 0; // ��������ȫ��ɾ������Ƶ��Ϊ0
                f.sign[abs(abs(f.sentences[i][0]) - 1)] = true; // ��������Ҫʹ��
                int result = detele_word(f, abs(f.sentences[i][0]));
                //�����Ժ�������ˣ��Ǿ������
                //�����Ժ󲻿գ��������˿��Ӿ䣬�Ǿ��Ǽٵ�
                if (result == true || result == false) {
                    return result;
                }// ������ˣ�
            }
            else if (f.sentences[i].size() == 0) //���ֿ��Ӿ䣬˵����������
            {
                return false; // �ù�ʽ��������
            }
        }
    }
    // ����Ϊֹ�жϲ��ˣ�˵���Ѿ�ɾ��������������
    return unknown;
}

// ���Ӿ�����е�ɾ������
int DPLL::detele_word(Formula& f, int need_to_delete) {
    for (int i = 0; i < f.sentences.size(); i++) {
        for (int j = 0; j < f.sentences[i].size(); j++) {
            //���ֿ��ܵ������Ҫɾ����1.���и����ֵ���ȡ��ʽ��2.���и����ֵķ񶨵õ���ȡ��ʽ
            if (need_to_delete == f.sentences[i][j]) { // ���и�����
                f.sentences.erase(f.sentences.begin() + i); // �Ƴ���iָ�����ȡ��ʽ
                i--;                // f.sentence.size()������һ�����Ϊ����ȫ������iҲ��һ
                if (f.sentences.size() == 0) //�Ƴ���������ʹ�øþ��ӳ�Ϊ���Ӿ䣬��ʱ�ù�ʽ������
                {
                    return true;
                }
                break;
            }
            else if (-need_to_delete == f.sentences[i][j]) // ���и����ֵķ�
            {
                f.sentences[i].erase(f.sentences[i].begin() + j); // �������ֵķ񶨴������ȡ��ʽ���Ƴ�
                j--;    // ɾ���˸÷��Ժ��ټ��һ�鿴���ܲ�����ɾ����һֱɾ������������������ֵķ�Ϊֹ
                if (f.sentences[i].size() == 0) // ��ʽ���ˣ��ǾͲ�������
                {
                    return false;
                }
            }
        }
    }
    // ��ʱ�����ǵİ취�޷��жϳ��Ƕ��Ǵ�����Ϊ������
    return unknown;
}

// ���ѹ���
int DPLL::work(Formula& f) {
    int result = single_sentence_rule(f); // ���õ��Ӿ��������ж�
    if (result == true || result == false) // �Ѿ����㣬��Ҳ�ò����ж�
    {
        return result;
    }
    // ����������Ķ�Ӧppt�е�L
    // ������һ�д��룬ȷ��L���������˭
    int L = distance(f.frequence_word.begin(), max_element(f.frequence_word.begin(), f.frequence_word.end())) + 1;
    // ���ѹ���
    Formula s1 = f, s2 = f;
    //�ȴ���s1

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
                }// ���һ�������г�����L���Ǿ;��Բ��ܳ���-L�����������
                s1.sentences[i].erase(s1.sentences[i].begin() + j);
                j--;
            }
            else if (s1.sentences[i][j] == -L) {
                for (int k = 0; k < s1.sentences[i].size(); k++) {
                    s1.frequence_word[abs(s1.sentences[i][k]) - 1]--;
                }//ע�⣡�����ڸþ��г��ֵ�ÿ�����֣���Ƶ�ʶ�Ҫ�Լ�����������ֻ��ȥL��Ƶ�ʣ�����
                s1.sentences.erase(s1.sentences.begin() + i);
                i--;//i ���ˣ�
                break;
            }
        }
    }

    //����s2
    for (int i = 0; i < s2.sentences.size(); i++) {
        for (int j = 0; j < s2.sentences[i].size(); j++) {
            if (s2.sentences[i][j] == -L) {
                s2.frequence_word[L - 1]--;
                for (int k = j + 1; k < s1.sentences[i].size(); k++) {
                    if (s1.sentences[i][k] == L)return false;
                }// ���һ�������г�����-L���Ǿ;��Բ��ܳ���L�����������
                s2.sentences[i].erase(s2.sentences[i].begin() + j);
                j--;
            }
            else if (s2.sentences[i][j] == L) {
                for (int k = 0; k < s2.sentences[i].size(); k++) {
                    s2.frequence_word[L - 1]--;
                }//ע�⣡�����ڸþ��г��ֵ�ÿ�����֣���Ƶ�ʶ�Ҫ�Լ�����������ֻ��ȥL��Ƶ�ʣ�����
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

//������������
void DPLL::output() {
    int result = work(formula);
    if (result == true) // ��ʽ������
    {
        cout << "SAT" << endl;
        cout << "please check the solution in the result.txt" << endl;
    }
    else // ��ʽ��������
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
    cout << endl << "����ʱ" << end_time - begin_time << "ms" << endl;
    return 0;
}