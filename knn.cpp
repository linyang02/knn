#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <queue>
#include <map>
#include <utility>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <cmath>
using namespace std;

struct comp {
    bool operator()(pair<double, int> a, pair<double, int> b) {
        return a.first > b.first;
    }
};

int classfier(vector<int>testdata, vector< vector<int> >traindata, vector<int>labels, int k) {
    if (traindata.empty() || testdata.empty()) {
        cout << "The testdata or traindata is empty" << endl;
        return -1;
    }
    if (testdata.size() != traindata[0].size()) {
        cout << "The testdata is invalid" << endl;
        return -1;
    }

    /* 求测试数据到每个训练数据的欧式距离 */
    // pair<double, int> 第一个double代表距离,第二个Int代表测试数据集的下标,下标和labels是一一对应
    priority_queue<pair<double, int>, vector<pair<double, int>>, comp> q;
    for (int index = 0; index < traindata.size(); index++) {
        long long sum = 0;
        for (int i = 0; i < traindata[index].size(); i++) {
            sum += pow((testdata[i] - traindata[index][i]), 2);
        }

        q.push(make_pair(sqrt(sum), index));
    }

    /* 选择距离最小的k个点, 并求出哪个数字出现的次数最多 */
    map<int, int> m; // key: 数字; value: 出现的次数
    for (int j = 0; j < k; j++) {
        pair<double, int> tmp = q.top();
        m[labels[tmp.second]]++; 
        q.pop();
    }

    int res = -1;
    int max = -1;
    for (auto it : m) {
        if (it.second > max) {
            max = it.second;
            res = it.first;
        } 
    }

    return res;
}

int main() {
    time_t start = time(NULL);
    int k = 0;
    string trainDir;
    string testDir;
    cout << "Please intput the K value: ";
    cin >> k;
    cout << "Please input the DIR of training digits: ";
    cin >> trainDir;
    cout << "Please input the DIR of testing digits: ";
    cin >> testDir;

    /* 读取训练数据和测试数据, 分别存放到1 * 1024的多维数组汇总 */
    vector<vector<int>> trainDigits;
    vector<vector<int>> testDigits;
    vector<int> trainlabels;
    vector<int> testlabels;
    DIR *dir;  
    struct dirent *ptr;

    // 读取训练数据,存放到多维数组中
    if ((dir=opendir(trainDir.c_str())) == NULL) {
        cout << "Open dir error..." << endl;
        return 0;
    }

    while ((ptr=readdir(dir)) != NULL) {
        if(ptr->d_type == 8) {
            trainlabels.push_back(ptr->d_name[0] - '0');

            string filename = trainDir + "/" + ptr->d_name;
            ifstream fin(filename);
            string line;
            vector<int> vec;
            while (getline(fin, line)) {
                for (int i = 0; i < 32; i++) {
                    vec.push_back(line[i] - '0');
                }
            }

            trainDigits.push_back(vec);
        }
    }
    closedir(dir);

    // 读取测试数据,存放到数组中
    if ((dir=opendir(testDir.c_str())) == NULL) {
        cout << "Open test dir error..." << endl;
        return 0;
    }

    while ((ptr=readdir(dir)) != NULL) {
        if(ptr->d_type == 8) {
            testlabels.push_back((int)ptr->d_name[0] - '0');

            string filename = testDir + "/" + ptr->d_name;
            ifstream fin(filename);
            string line;
            vector<int> vec;
            while (getline(fin, line)) {
                for (int i = 0; i < 32; i++) {
                    vec.push_back(line[i] - '0');
                }
            }

            testDigits.push_back(vec);
        }
    }
    closedir(dir);

    int correct_num = 0;
    int error_num = 0;
    cout << "trainDigits: " << trainDigits.size() << endl;
    cout << "testDigits: " << testDigits.size() << endl;

    for (int tt = 0; tt < testDigits.size(); tt++) {
        int ret = classfier(testDigits[tt], trainDigits, trainlabels, k);
        if (ret == testlabels[tt]) {
            correct_num++;
        } else {
            error_num++;
        }
    }

    double rate = (double)correct_num / (correct_num + error_num);

    cout << "correct_num: " << correct_num << endl;
    cout << "error_num: " << error_num << endl;
    cout << "Correct rate: " << rate << endl;
    time_t end = time(NULL);
    cout << "time used: " << end - start << "s" << endl;

    return 0;
    
}
