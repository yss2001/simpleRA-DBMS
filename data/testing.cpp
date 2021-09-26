#include <bits/stdc++.h>

using namespace std;

struct sparseNode
{
	int row, column, value;
};

int main()
{
    fstream fout;
    int outvar = 0,cnt = 0;
    while(true)
    {
        fout.open("./temp/ms_Page" + to_string(cnt),ios::in | ios::out | ios::binary);
        sparseNode node;
        cout<<"\n---- Page "<<cnt<<" -----\n\n";
		while (fout.read(reinterpret_cast<char *>(&node), sizeof(node)))
		{
            cout<<node.row<<" "<<node.column<<" "<<node.value<<endl;
        }
        cnt++;
        cin>>outvar;
        if(outvar == 0)
            break;
    }
}