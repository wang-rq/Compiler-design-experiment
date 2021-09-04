#include <iostream>
#include <stack>
#include <vector>
#include <iomanip>
#include <math.h>
using namespace std;

struct Node
{
    string val;
    struct Node *left;
    struct Node *right;
};

class BinaryTree
{
public:
    BinaryTree(struct Node *r = NULL) : root(r) {}
    BinaryTree(vector<string> &v);
    ~BinaryTree();
    int getDepth(Node *root);
    void write(vector<vector<string>> &print, Node *root, int cur_depth, int left, int right);
    vector<vector<string>> get_tree_vec(Node *root);
    void print_tree();
    void deleteNode(struct Node *pNode);

private:
    stack<struct Node *> st;
    struct Node *root;
    int width;
};

//构造函数
BinaryTree::BinaryTree(vector<string> &v)
{
    for (unsigned i = 0; i < v.size(); ++i)
    {
        // 新建一个节点，并赋值
        struct Node *pNode = new Node;
        pNode->val = v[i];
        pNode->left = NULL;
        pNode->right = NULL;

        // 如果是运算符，就弹出并且作为刚才新建的节点的左右孩子
        if ((v[i] == "+") || (v[i] == "-") || (v[i] == "*") || (v[i] == "/"))
        {
            pNode->right = st.top();
            st.pop();
            pNode->left = st.top();
            st.pop();
        }
        // 将新树压栈
        st.push(pNode);
    }
    //循环结束后，最后根节点指针会是栈顶指针
    root = st.top();
}

//析构函数
BinaryTree::~BinaryTree()
{
    deleteNode(root);
}

void BinaryTree::deleteNode(struct Node *pNode)
{
    if (pNode != NULL)
    {
        delete pNode;
        deleteNode(pNode->left);
        deleteNode(pNode->right);
    }
}

int BinaryTree::getDepth(Node *root)
{
    if (!root)
    {
        return 0;
    }
    return max(getDepth(root->left), getDepth(root->right)) + 1;
}

void BinaryTree::write(vector<vector<string>> &print, Node *root, int cur_depth, int left, int right)
{
    if (!root)
    {
        return;
    }
    print[cur_depth][left] = root->val;
    //递归深度优先遍历
    write(print, root->left, cur_depth + 1, left, (left + right) / 2);
    write(print, root->right, cur_depth + 1, (left + right) / 2, right);
}

vector<vector<string>> BinaryTree::get_tree_vec(Node *root)
{
    int row = getDepth(root);
    int col = 2 << row - 1;
    vector<vector<string>> print(row, vector<string>(col, ""));
    write(print, root, 0, 0, col);
    return print;
}

void BinaryTree::print_tree()
{
    vector<vector<string>> ans = get_tree_vec(root);
    vector<int> empty(32, 0);
    for (int i = 0; i > empty.size(); i++)
        cout << empty[i] << endl;
    for (int i = 0; i < ans.size(); i++)
    {
        for (int j = 0; j < ans[i].size(); j++)
        {
            if (ans[i][j] != "")
                empty[j] = 1;
        }
    }
    for (int i = 0; i < ans.size(); i++)
    {
        for (int j = 0; j < ans[i].size(); j++)
        {
            //cout << '(' << ans[i][j]<<')'<<' ';
            if (empty[j] != 0)
                cout << setiosflags(ios::left) << setw(5) << ans[i][j];
        }
        cout << endl;
    }
}

int get_priority(char ch) //判断符号的优先级
{
    if (ch == '*' || ch == '/')
        return 2;
    else if (ch == '+' || ch == '-')
        return 1;
    else
        return 0;
}

string get_postfix(string infix)
{
    stack<char> char_stack;
    string postfix;
    int lenth = infix.size();
    for (int i = 0; i < lenth; i++) //从左往右遍历中缀表达式
    {
        if (isdigit(infix[i])) //如果是数字
        {
            postfix += infix[i];
            //如果这个数字结束了，后缀表达式中加空格
            if (i == lenth - 1)
                postfix += ' ';
            else if (!isdigit(infix[i + 1]))
                postfix += ' ';
        }
        else if (infix[i] == '(') //如果左括号直接进栈
        {
            char_stack.push(infix[i]);
        }
        else if (infix[i] == ')') //如果右括号，一直出栈直到遇到左括号
        {
            while (char_stack.top() != '(')
            {
                postfix = postfix + char_stack.top() + ' ';
                char_stack.pop();
            }
            char_stack.pop();
        }
        else if (infix[i] == '+' || infix[i] == '-' || infix[i] == '*' || infix[i] == '/')
        {
            if (char_stack.empty()) //如果空栈就push进栈
            {
                char_stack.push(infix[i]);
            }
            else //如果不空，与栈顶符号比较优先级
            {
                while (!char_stack.empty())
                {
                    if (get_priority(infix[i]) <= get_priority(char_stack.top())) //如果优先级低于等于栈顶的符号，栈顶元素就出栈直到不满足条件
                    {
                        postfix = postfix + char_stack.top() + ' ';
                        char_stack.pop();
                    }
                    else
                        break;
                }
                char_stack.push(infix[i]); //进栈
            }
        }
    }
    while (!char_stack.empty())
    {
        postfix = postfix + char_stack.top() + ' ';
        char_stack.pop();
    }
    return postfix;
}

// 将后缀表达式分割成 vector 中的元素
int splitStringToVect(const string &srcStr, vector<string> &destVect, const string &strFlag)
{
    int pos = srcStr.find(strFlag, 0);
    int startPos = 0;
    int splitN = pos;
    string lineText(strFlag);

    while (pos > -1)
    {
        lineText = srcStr.substr(startPos, splitN);
        startPos = pos + 1;
        pos = srcStr.find(strFlag, pos + 1);
        splitN = pos - startPos;
        destVect.push_back(lineText);
    }

    lineText = srcStr.substr(startPos, srcStr.length() - startPos);
    return destVect.size();
}

int main()
{
    while (1)
    {
        string infix, postfix; //中缀，后缀
        cin >> infix;
        if (infix == "EXIT")
            break;
        postfix = get_postfix(infix);
        vector<string> v;
        splitStringToVect(postfix, v, " ");
        BinaryTree tree(v);
        tree.print_tree();
        cout << endl;
    }
}