#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <queue>

using namespace std;
const int INF = 1000000000;
vector<int> lev;				//����� ���������� ����� �� ������ s �� ������ �������
vector<vector<int>> NewAdj;
vector<int> pred;

struct Top
{
	int x, y;
	Top(int a, int b): x(a), y(b)
	{}
};

struct edge {
	int a, b, capacity, flow;
	bool const operator==(const edge & second) const {
		return (a == second.a) && (b == second.b) && (capacity == second.capacity) && (flow == second.flow);
	}
};
vector<edge> e;
bool findIne(edge x)
{
	for(int i = 0; i != e.size(); ++i)
		if(x == e[i])
			return true;
	return false;
}

void readPvkOrPin(ifstream &input_file, int N, int K, vector<int> &A)
{
	int i, temp;
	for(i = 0; i < K; ++i)
		input_file >> A[i];
	for(i = 0; i < K; ++i)
	{
		input_file >> temp;
		A[i] *= N;
		A[i] += temp;
	}
}
void makeAdjList(int N, vector<vector<int>> &AdjList)
{
	int i, j;
	long bigN = N * N;
	AdjList.resize(2*bigN + 2);
	for(i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
		{
			//�������������� ����
			//cout << "hi4"<<endl;
			AdjList[i * N + j + bigN].push_back(i * N + j);

			//���� �����������
			if (i != 0 && j != 0 && i != (N-1) && j != (N-1))
			{//cout << "hi1"<<endl;
				//��� �������, �� ������� ��� ���� �������(0..N^2-1)
				//���� �����
				AdjList[i * N + j].push_back(i * N + j - 1);
				//���� ������
				AdjList[i * N + j].push_back((i - 1) * N + j);
				//���� ������
				AdjList[i * N + j].push_back(i * N + j + 1);
				//���� �����
				AdjList[i * N + j].push_back((i + 1) * N + j);

				//��� �������, � ������� ��� ���� ������(N^2..2N^2-1)
				//���� �����
				AdjList[i * N + bigN + j - 1].push_back(i * N + bigN + j);
				//���� ������
				AdjList[(i - 1) * N + bigN + j].push_back(i * N + bigN + j);
				//���� ������
				AdjList[i * N + bigN + j + 1].push_back(i * N + bigN + j);
				//���� �����
				AdjList[(i + 1) * N + bigN + j].push_back(i * N + bigN + j);
			}
			//������� ������ � ������
			if((i == 0 || i == (N - 1)) && j != (N-1))
			{
				//cout << "hi2"<<endl;
				//������ ���� ������
				//��� �������, �� ������� ��� ���� �������(0..N^2-1)
				AdjList[i * N + j].push_back(i * N + j + 1);
				//��� �������, � ������� ��� ���� ������(N^2..2N^2-1)
				AdjList[i * N + bigN + j + 1].push_back(i * N + bigN + j);
			}
			//����� ������� � ������ �������
			if((j == 0 || j == (N-1))&& i != (N-1))
			{
				//cout << "hi3"<<endl;
				//������ ����
				//��� �������, �� ������� ��� ���� �������(0..N^2-1)
				AdjList[i * N + j].push_back((i + 1) * N + j);
				//��� �������, � ������� ��� ���� ������(N^2..2N^2-1)
				AdjList[(i + 1) * N + bigN +j].push_back(i * N + bigN + j);
			}
		}
}
void makeAdjList2(int N, vector<vector<int>> &AdjList)
{
	int i, j;
	long bigN = N * N;
	AdjList.resize(2*bigN + 2);
	for(i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
		{
			//�������������� ����
			//cout << "hi4"<<endl;
			AdjList[i * N + j /*+ bigN*/].push_back(i * N + j + bigN);
			//����� ���� �����
			if(i > 0)
			{
				//AdjList[i * N + j].push_back((i - 1) * N + j);
				AdjList[(i - 1) * N + bigN + j].push_back(i * N + bigN + j);
			}
			//����� ���� ����
			if(i < (N-1))
			{
				//AdjList[i * N + j].push_back((i + 1) * N + j);
				AdjList[(i + 1) * N + bigN + j].push_back(i * N + bigN + j);
			}
			//����� ���� �����
			if(j > 0)
			{
				//AdjList[i * N + j].push_back(i * N + j - 1);
				AdjList[i * N + bigN + j - 1].push_back(i * N + bigN + j);
			}
			//����� ���� ������
			if(j < (N-1))
			{
				//AdjList[i * N + j].push_back(i * N + j + 1);
				AdjList[i * N + bigN + j + 1].push_back(i * N + bigN + j);
			}
		}
	//������ ������ ����� �������� � ���������

}
void makeAdjList3(int N, vector<vector<int>> &AdjList, vector<int> A)
{
	int i, j, number = -1;
	long bigN = N * N;
	AdjList.resize(bigN + A.size());
	for(i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
		{
			int elem = i * N + j;
			bool flag = false;
			for( int b = 0; b != A.size(); ++b)
				if(elem == A[b])
				{
					flag = true;
					number = b;
				}
						
			if(flag)
			{
				//�������������� ����
				cout << "hi4"<<endl;
				AdjList[number + bigN].push_back(elem);
				AdjList[elem -1].push_back(number + bigN); //�����
				AdjList[elem - N].push_back(number + bigN); //�������
				AdjList[elem + 1].push_back(number + bigN); //������
				AdjList[elem + N].push_back(number + bigN); //������
			}
			//����� ���� �����
			if(i > 0)
			{
				AdjList[elem].push_back((i - 1) * N + j);
				//AdjList[(i - 1) * N + bigN + j].push_back(i * N + bigN + j);
			}
			//����� ���� ����
			if(i < (N-1))
			{
				AdjList[elem].push_back((i + 1) * N + j);
				//AdjList[(i + 1) * N + bigN + j].push_back(i * N + bigN + j);
			}
			//����� ���� �����
			if(j > 0)
			{
				cout<< "Hi 2"<<endl;
				AdjList[elem].push_back(elem - 1);
				//AdjList[i * N + bigN + j - 1].push_back(i * N + bigN + j);
			}
			//����� ���� ������
			if(j < (N-1))
			{
				AdjList[elem].push_back(elem + 1);
				//AdjList[i * N + bigN + j + 1].push_back(i * N + bigN + j);
			}
		}
}
void makeAdjList4(int N, vector<vector<int>> &AdjList, vector<int> &Pvk, vector<int> &Pin)
{
	int i, j;
	long bigN = N * N;
	AdjList.resize(2*bigN + 2);
	for(i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
		{
			int element = i * N + j;
			//�������������� ����
			AdjList[element].push_back(element + bigN);
			//����� ���� �����
			if(i > 0)
			{
				//AdjList[i * N + j].push_back((i - 1) * N + j);
				AdjList[element + bigN].push_back((i-1) * N + j);
			}
			//����� ���� ����
			if(i < (N-1))
			{
				//AdjList[i * N + j].push_back((i + 1) * N + j);
				AdjList[element + bigN].push_back((i + 1) * N + j);
			}
			//����� ���� �����
			if(j > 0)
			{
				//AdjList[i * N + j].push_back(i * N + j - 1);
				AdjList[element + bigN].push_back(i * N + j - 1);
			}
			//����� ���� ������
			if(j < (N-1))
			{
				//AdjList[i * N + j].push_back(i * N + j + 1);
				AdjList[element + bigN].push_back(i * N + j + 1);
			}
		}
	//������ ������ ����� �������� � ��������� 
	//������������� ��� ��� ���������
	int K = Pvk.size();
	for(i = 0; i < K; ++i)
		AdjList[2 * bigN].push_back(Pvk[i]);
	//��������� ��� ��� ���������
	for(i = 0; i < K; ++i)
		AdjList[Pin[i] + bigN].push_back(2 * bigN + 1);

}

void add_edge (int a, int b, vector<vector<int>> &Adj)
{
	edge e1 = { a, b, 1, 0 };		//capacity = 1
	edge e2 = { b, a, 0, 0 };
	Adj[a].push_back ((int) e.size());
	e.push_back (e1);
	Adj[b].push_back ((int) e.size());
	e.push_back (e2);
}
//c������ ������ ��������� � ������� � ����� � ��������� ������
void makeNewAdjWithEdge(vector<vector<int>> &AdjList)
{
	int Size = AdjList.size();
	NewAdj.resize(Size);
	for(int i = 0; i < Size; ++i)
		for(size_t j = 0; j < AdjList[i].size(); ++j)
			add_edge(i, AdjList[i][j], NewAdj);
}
//c����� �������� ����
bool bfs(int N) 
{
	//cout<<"bfs"<<endl;
	//�.� �� �������������� lev = -1, �� used �� �����, ������ ������ �������
	int bigN = 2 * N * N + 2;
	lev.assign(bigN,-1);				
	int id, to;
	int s = bigN - 2;					//�������������
	int t =	bigN - 1;					//c��������	
	queue<int> Q;
	Q.push(s);
	lev[s] = 0;			
	
	while (!Q.empty() && lev[t] == -1)
	{
		int v = Q.front();
		Q.pop();
		for (size_t i = 0; i < NewAdj[v].size(); ++i)
		{
			id = NewAdj[v][i];
			to = e[id].b;
			if ((lev[to] == -1) && (e[id].flow < e[id].capacity))
			{
				Q.push(to);
				lev[to] = lev[v] + 1;
			}
		}
	}
	return lev[t] != -1;
}
//���� ������������� �����
int dfs (int N, int v, int flow)
{
	int id, to, pushed;
	int t = 2 * N * N + 1;
	if (!flow)  
		return 0;
	if (v == t)  
		return flow;
	for (int i = 0; i < (int)NewAdj[v].size(); ++i)
	{
		
		id = NewAdj[v][i];				
		to = e[id].b;
		if (lev[to] != lev[v] + 1)
			continue;
		//if(e[id].flow > 0)
		//if(pred[to] == -1)
			pred[to] = v;
		//else
			//pred[to].push_back(v);
		//pred[i] = id;
		pushed = dfs (N, to, min(flow, e[id].capacity - e[id].flow));
		//cout<< pushed <<endl;
		if (pushed) 
		{
			
			e[id].flow += pushed;		//id^1 = xor 1
			e[id^1].flow -= pushed;		//��� �������� �������� �����, ��� ��� ����� � e ����������� 
			return pushed;				//������� ������ � ������ �������, ����� �������� � ��������.	
		}
	}
	return 0;
} 
//�������� ������� ��������� ������
int Dinic(int N)
{
	//cout <<"dinic"<<endl;
	int pushed, flow = 0;
	int bigN = 2 * N * N + 2;
	int s = bigN - 2; 
	/*while(true)
	{
		if (!bfs(N)) 
			break;*/
	while(bfs(N))
	{
		pred.assign(bigN, 0);
		while (pushed = dfs (N, s, INF))
			flow += pushed;
	}
	return flow;
}
void findPath(ofstream &output_file, vector<int> &Pin, vector<int> &Pvk, int K, int N)
{
	cout<<"path"<<endl;
	vector<int> top;
	int i, j, temp;// num;
	int size = N * N;
	for(i = 0; i < K; ++i)
	{
		/*output_file << Pvk[i] << " ";
		for(j = 0; j < pred.size(); ++ j)
			if(pred[j] == Pvk[i])
				temp = j;*/
		//num = 0;
		temp = Pin[i];
		top.push_back(temp);
		while(temp != Pvk[i] && temp != pred[temp])
		{
			//cout<<"temp = "<< temp << endl;
			
			/*for(j = 0; j < pred[temp].size(); ++j)
			{
				edge element;
				element.a = pred[temp][j];
				element.b = temp;
				element.capacity = 1;
				element.flow = 1;
				if(findIne(element))
				{
					temp = pred[temp][j];*/
					temp = pred[temp];
					if(temp < size)				//��� �� ����� ������ �������, ������� �� ��������
						top.push_back(temp);
				//}
			//}
				
		}
		int s = top.size()-1;
		for(j = s; j != -1; --j)
		{
			output_file << top[j] << " ";
			//top[j].pop_back();
		}
		output_file << endl;
		top.assign(0,0);
	}

}
int main()
{
	int N, K, i, j;
	
	ifstream input_file;
	input_file.open("input.txt");
	map<Top, int> num;
	vector<vector<int>> AdjList;
	//��������� ������
	input_file >> N;
	//���������� ��� � �����
	input_file >> K;

	/*for (i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
		{
			Top element(i, j);
			num[element] = i;
		}*/
	//vector<Top> Pvk(K);
	//vector<Top> Pin(K);
	/*for(i = 0; i < K; ++i)
		input_file >> Pvk[i].x;
	for(i = 0; i < K; ++i)
		input_file >> Pvk[i].y;
	for(i = 0; i < K; ++i)
		input_file >> Pin[i].x;
	for(i = 0; i < K; ++i)
		input_file >> Pin[i].y;*/
	
	vector<int> Pvk(K);
	vector<int> Pin(K);
	readPvkOrPin(input_file, N, K, Pvk);
	readPvkOrPin(input_file, N, K, Pin);
	
	makeAdjList4(N, AdjList, Pvk, Pin);

	int bigSize = 2 * N * N + 2;
	for(i = 0; i < bigSize; ++i)
	{
		cout << "i = "<< i << "| ";
		int size = AdjList[i].size();
		for(j = 0; j < size; ++j)
			cout << AdjList[i][j] << " ";
		cout << endl;
	}
	

	makeNewAdjWithEdge(AdjList);
	for(i = 0; i < bigSize; ++i)
	{
		//cout<<"print" << endl;
		cout << "i = "<< i << "| ";
		int size = NewAdj[i].size();
		for(j = 0; j < size; ++j)
			cout << NewAdj[i][j] << " ";
		cout << endl;
	}
	int result = Dinic(N);

	cout << "Predcessors"<<endl;
	/*for(i = 0; i < pred.size(); ++i)
	{
			cout << "i = "<< i << " | ";
		for(j = 0; j < pred[i].size(); ++j)
			cout << pred[i][j] <<" ";
		cout<< endl;
	}*/
	for(i = 0; i < pred.size(); ++i)
	{
			cout << "i = "<< i << " | " << pred[i] <<" "<< endl;
	}
	cout<<"vector e"<<endl;
	for(i = 0; i != e.size();++i)
		cout << "i = "<< i <<" | " <<e[i].a <<"->"<<e[i].b<<" cap = "<<e[i].capacity<<", f = "<<e[i].flow<<endl;
	ofstream output_file;
	output_file.open("output.txt");

	if (result < K)
	{
		output_file << -1;
		return 0;
	}
	else
		findPath(output_file, Pin, Pvk, K, N);
		
	input_file.close();
	output_file.close();
	return 0;
}