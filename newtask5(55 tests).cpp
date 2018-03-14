//Task5
#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <queue>
#include <set>
#include <cmath>

using namespace std;

const double eps = 1e-9;
const double Pi = 3.1415926;
const int INF = 1000000000;

int s, t;			//стартовая и конечная вершина

struct intPoint
{
	double x, y;
	/*bool const operator<(const intPoint & second) const {
		return (x < second.x && y < second.y) || (x == second.x && y < second.y);
	}*/

	/*point(int a, int b): x(a), y(b)
	{}*/
};
intPoint Vintik;
intPoint Shpuntik;
struct doublePoint
{
	double x, y;
};

struct cross
{
	double x, y, number;
	//vector<int> Nline;
	/*cross(int a, int b, int c): x(a), y(b), Nline(c)
	{}*/
};
//чтобы в нужном порядке выводил слева направо сегменты, так удобнее=)
struct cmp
{
	bool operator ()(const pair<double, double> &a, const pair<double, double> &b)
    {
		return (a.first > b.first) || (abs(a.first - b.first) < eps && a.second > b.second);
    }
};
struct line
{
	int A, B, C;
};
struct Segment
{
	pair<double, double> cross1, cross2;
	int Nseg;
	int Nline;
	line l;
	pair<double, double> prev;
	bool Vint, Shpunt;
};
map<pair<double, double>, vector<Segment>> CrossToSegment;

struct Vector
{
	double x, y;
	//вычисление вектора по координатам двух точек
	Vector(pair<double, double> &P1, pair<double, double> &P2):x(P2.first - P1.first), y(P2.second - P1.second)
	{}
};
//есть ли на сегменте кто-то
void isSmbHere(Segment &S)
{
	//Шпунтик
	if((S.cross1.first == Shpuntik.x && S.cross1.second == Shpuntik.y) || (S.cross2.first == Shpuntik.x && S.cross2.second == Shpuntik.y))
	{
		S.Shpunt = true;
		t = S.Nseg;
	}
	else
		S.Shpunt = false;
	//Винтик
	if(((int)S.cross1.first == Vintik.x && S.cross1.second == Vintik.y) || (S.cross2.first == Vintik.x && S.cross2.second == Vintik.y))
	{
		S.Vint = true;
		s = S.Nseg;
	}
	else
		S.Vint = false;
}
void makeLines(vector<vector<int>> &Roads, vector<line> &Lines)		
{
	int N, i;
	N = Roads.size();
	Lines.resize(N);
	for(i = 0; i < N; ++i)
	{
		Lines[i].A = Roads[i][1] - Roads[i][3];			//y1-y2
		Lines[i].B = Roads[i][2] - Roads[i][0];			//x2 - x1
		Lines[i].C = -Lines[i].A * Roads[i][0] - Lines[i].B * Roads[i][1];	//-A*x1-B*y1
	}
}
void createSegments(vector<line> &L, vector<Segment> &Segments, bool &flag)
{
	int i, j, k, number = 0;
	bool VintikFounded = false,
	ShpuntikFounded = false;
	int N = L.size();
	//для каждой линии топаем
	for(i = 0; i < N; ++i)
	{
		priority_queue<pair<double, double>, vector<pair<double,double>>, cmp> Pr; 
		map<pair<double, double>, int> used;

		bool flag1(0), flag2(0);
		//если Шпунтик на линий
		bool temp1 = (L[i].A * Shpuntik.x + L[i].B * Shpuntik.y + L[i].C == 0);
		if(temp1)
		{
			flag1 = true;
			Pr.push(make_pair(Shpuntik.x, Shpuntik.y));
		}
		//если Винтик на линии
		bool temp2 = (L[i].A * Vintik.x + L[i].B * Vintik.y + L[i].C == 0);
		if(temp2)
		{
			flag2 = true;
			Pr.push(make_pair(Vintik.x, Vintik.y));
		}

		for(j = 0; j < N; ++j)
		{
			if(j != i)
			{
				double z = L[i].A * L[j].B - L[j].A * L[i].B;
				if(z != 0) //пересекаются
				{
					pair<double, double> element;
					element.first = (L[i].B * L[j].C - L[j].B * L[i].C)/z; //(B1C2-B2C1)/(A1B2-A2B1)
					element.second = (L[i].C * L[j].A - L[j].C * L[i].A)/z; //(C1A2-C2A1)/(A1B2-A2B1)
					
					if(used.find(element) == used.end()) 
					{
						Pr.push(element);
						used[element] = j;
					}
				}
			}
		}
		if(Pr.empty())
		{
			flag = false;
			break;
		}
		//формируем сегменты
		int size = Pr.size();			
		for(k = 0; k < size - 1; ++k)
		{
			Segment s;
			s.cross1 = Pr.top();
			Pr.pop();
			s.cross2 = Pr.top();
			s.Nline = i; 
			s.l = L[i];
			s.Nseg = number++;
			s.prev = make_pair(INF, INF);
			isSmbHere(s);
			Segments.push_back(s);
			//запоминаем в перекрестках, какому сегменту они принадлежат
			CrossToSegment[s.cross1].push_back(s);
			CrossToSegment[s.cross2].push_back(s);
		}
		Pr.pop();
		used.clear();
	}
}
void findCorner(pair<double, double> &Common, pair<double, double> &P1, pair<double, double> &P2, double &angle)
{
	Vector F(Common, P1);//, S;
	Vector S(Common, P2);
	double Mul, M1, M2, a;
	Mul = F.x * S.x + F.y * S.y;
	M1 = sqrt(F.x * F.x + F.y * F.y);
	M2 = sqrt(S.x * S.x + S.y * S.y);
	a = Mul / M1 / M2;
	if (a > 1 || a < -1)
		angle = 180;
	else
	{
			a = acos(a);
			angle = 180 * a/Pi;
	}
}
void makeAdjList(vector<Segment> &Segments, vector<vector<pair<int, double>>> &Adj)
{
	int i, j;
	int Size = Segments.size();
	Adj.resize(Size);
	for(i = 0; i < Size ; ++i)
	{
		vector<Segment> b;
		int curN = Segments[i].Nseg;
		//первый перекресток сегмента смотрим
		b = CrossToSegment[Segments[i].cross1];
		for(j = 0; j < b.size(); ++j)
		{
			if(b[j].Nseg != curN)
			{
				double angle = 0;
				pair<double, double> second;
				if(Segments[i].cross1 == b[j].cross1)
					second = b[j].cross2;
				else
					second = b[j].cross1;
				findCorner(Segments[i].cross1, Segments[i].cross2, second, angle);
				if(angle > eps)
					angle = 180 - angle;
				Adj[curN].push_back(make_pair(b[j].Nseg, angle));
			}
		}
		//второй перекресток сегмента смотрим
		b = CrossToSegment[Segments[i].cross2];
		for(j = 0; j < b.size(); ++j)
		{
			if(b[j].Nseg != curN)
			{
				double angle = 0;
				pair<double, double> second;
				if(Segments[i].cross2 == b[j].cross1)
					second = b[j].cross2;
				else
					second = b[j].cross1;
				findCorner(Segments[i].cross2, Segments[i].cross1, second, angle);
				if(angle > eps)
					angle = 180 - angle;
				Adj[curN].push_back(make_pair(b[j].Nseg, angle));
			}
		}
	}
}
void findCommonCross(Segment &F, Segment &S, pair<double, double> &C)
{
	if (F.cross1 == S.cross1 || F.cross1 == S.cross2)
		C = F.cross1;
	else
		C = F.cross2;
}
void Dijkstra(int N, vector<Segment> &S, vector<vector<pair<int, double>>> &Adj,vector <double> &d, vector <bool> &used)
{
	int i, j, v, to;
	double len;
	pair<double, double> Common;
	for (i = 0; i < N; ++i)
	{
		v = -1;
		for (j = 0; j < N; ++j)
			if (!used[j] && (v == -1 || d[j] < d[v]))
				v = j;
		if (d[v] == INF)
			break;
		used[v] = true;
		for (j = 0; j < (int)Adj[v].size(); ++j)
		{
			to = Adj[v][j].first;
			findCommonCross(S[v], S[to], Common);		//из такого пришли перекрестка
			if (S[v].prev == Common)
				continue;
			len = Adj[v][j].second;
			if (d[v] + len < d[to])
			{
				S[to].prev = Common;	
				d[to] = d[v] + len;
			}
		}
	}
}

int main()
{
	int N, i, j;
	
	ifstream input_file;
	input_file.open("input.txt");
	vector<vector<int>> Roads;
	vector<line> Lines;
	vector<Segment> Segments;
	vector<vector<pair<int, double>>> AdjList;
	vector <bool> used;
	vector <double> d;  // кратчайшие расстояния до вершин

	//считываем количество дорог
	input_file >> N;
	//cчитываем дороги
	Roads.resize(N);
	//cout << " Roads" << endl;
	for(i = 0; i < N; ++i)
	{
		Roads[i].resize(4);
		for(j = 0; j < 4; ++j)
		{
			input_file >> Roads[i][j];
			//cout << Roads[i][j] << " ";
		}
		//cout << endl;
	}
	//считываем Винтика
	input_file >> Vintik.x >> Vintik.y;
	//cout << "Vintik : ("<< Vintik.x << ", " << Vintik.y << ");" ;
	//cout << endl;
	//считываем Шпунтика
	input_file >> Shpuntik.x >> Shpuntik.y;
	//cout << "Shpuntik : ("<< Shpuntik.x << ", " << Shpuntik.y << ");"<< endl;
	
	ofstream output_file;
	output_file.open("output.txt");
	bool flag = true;

	makeLines(Roads, Lines);
	createSegments(Lines, Segments, flag); 
	if(flag)
	{
		makeAdjList(Segments, AdjList);

		//ура, можно Дейкстру запускать
		int Size = Segments.size(); 
		d.assign(Size, INF);
		used.assign(Size, 0);
		d[s] = 0;					//cтартовая вершина, кратчайшее расстояние = 0
		Dijkstra(Size, Segments, AdjList, d, used);
	



	//int it;
	//for(it = 0; it != Segments.size(); ++it)
	//{
	//	cout << "Nline = " << Segments[it].Nline << " | ";
	//	cout <<"(" << Segments[it].cross1.first << ", "<< Segments[it].cross1.second<< ") -> (";
	//	cout << Segments[it].cross2.first << ", " << Segments[it].cross2.second << ") ;" ;
	//	cout << "Spunt?-"<< Segments[it].Shpunt << ", Vint?-"<< Segments[it].Vint ;
	//	cout << "  num = "<< Segments[it].Nseg<< endl;
	//	
	//	/*for(j = 0; j < Crosses[i].Nline.size(); ++j)			//забей
	//		cout << Crosses[i].Nline[j] << " ";*/
	//	//cout << endl;
	//}
	
	/*cout<<"Adj"<<endl;
	for(i = 0 ; i < AdjList.size(); ++i)
	{
		cout << "i = " << i << " | ";
		for(int w = 0; w < AdjList[i].size(); ++w)
		{
			cout << "("<< AdjList[i][w].first << ", " << AdjList[i][w].second << ") ";
		}
		cout << endl;
	}*/
	
	
	

		//cout << "result = " << d[t] << endl;
		output_file << d[t] ;
	}
	else
		output_file << -1;
		
	input_file.close();
	output_file.close();
	return 0;
}