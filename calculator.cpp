#include<iostream>
#include<string>
#include<stack>
#include<stdexcept>
#include<cctype>
#include<cmath>

double calculate(double, double, char);
int precedence(char op);
void applyTopOperator(std::stack<double>& numbers, std::stack<char>& operators);
double evaluateExpression(const std::string& expression);
double parseNumber(const std::string& expression, std::size_t& i);

// 进行计算
double calculate(double a, double b, char op)
{
	switch (op)
	{
	case'+':
		return a + b;
	case'-':
		return a - b;
	case'*':
		return a * b;
	case'/':
		// 判断除数是否为零，若是零则抛出异常
		if (b == 0)
		{
			throw std::runtime_error("Dividing by zero");
		}
		return a / b;
	default:
		throw std::invalid_argument("Unknown operator");
	}
}

// 实现运算符的优先级,返回的数值越大，优先级越高
int precedence(char op)
{
	switch(op)
	{
	case'+':
	case'-':
		return 1;
	case'*':
	case'/':
		return 2;
	default:
		return -1;
	}
}

// 从两个栈中取出两个数字和一个运算符，再把计算后的结果重新压回栈中
void applyTopOperator(std::stack<double>& numbers, std::stack<char>& operators)
{
	// 检查数字栈中至少有两个元素，运算符栈至少有一个元素
	if (numbers.size() < 2 || operators.empty())
	{
		throw std::runtime_error("stacks not have enough element");
	}
	else
	{
		// 取出栈顶运算符
		char op = operators.top();
		operators.pop();

		// 取出右操作数和左操作数
		double right = numbers.top();
		numbers.pop();
		double left = numbers.top();
		numbers.pop();

		// 调用函数进行计算
		double result = calculate(left, right, op);

		//将计算的结果压回数字栈中
		numbers.push(result);
	}
}

// 处理负数
double parseNumber(const std::string& expression, std::size_t& i)
{
	std::size_t length = 0;
	double value = std::stod(expression.substr(i), &length);
	i += length;
	return value;
}

double evaluateExpression(const std::string& expression)
{
	std::stack<double> numbers;
	std::stack<char> operators;

	std::size_t i = 0;
	bool expectNumber = true;

	while (i < expression.size())
	{
		// 跳过空白字符
		if (std::isspace(static_cast<unsigned char>(expression[i])))
		{
			++i;
			continue;
		}

		// 从当前位置提取字符串判断是否是abs/sqrt
		if (std::isalpha(static_cast<unsigned char>(expression[i])))
		{
			std::string func;
			if (expression.compare(i, 3, "abs") == 0)
			{
				// 记录函数名并移动i
				func = expression.substr(i, 3);
				i += 3;
			}
			else if (expression.compare(i, 4, "sqrt") == 0)
			{
				// 记录函数名并移动i
				func = expression.substr(i, 4);
				i += 4;
			}
			else
			{
				throw std::invalid_argument("Invalid parameter");
			}

			if (i >= expression.size() || expression[i] != '(')
			{
				throw std::invalid_argument("Expected '(' after function name");
			}
			else
			{
				++i;
				std::size_t start = i;
				// 寻找右括号，提取两个括号中的表达式
				std::size_t end = expression.find(')', start);
				if (end == std::string::npos)
				{
					throw std::invalid_argument("Missing ')'");
				}
				else
				{
					if (!expectNumber)
					{
						throw std::invalid_argument("Missing operator before function");
					}


					std::string argument = expression.substr(start, end - start);
					i = end + 1;
					double value = evaluateExpression(argument);
					double result;

					if (func == "abs")
					{
						result = std::abs(value);
					}
					else if (func == "sqrt")
					{
						if (value < 0)
						{
							throw std::invalid_argument("sqrt argument cannot be negative");
						}
						result = std::sqrt(value);
					}

					numbers.push(result);
					expectNumber = false;
					continue;
				}
			}
		}

		// 如果当前是数字或小数点
		if (std::isdigit(static_cast<unsigned char>(expression[i])) || expression[i] == '.'||(expectNumber&&expression[i]=='-')||(expectNumber && expression[i] == '+'))
		{
			// 检查当前是否应该读取数字
			if (!expectNumber)
			{
				throw std::invalid_argument("Missing operator");
			}
			// 使用std::stod提取数字
			std::size_t length = 0;
			double value = std::stod(expression.substr(i), &length);
			//std::cout << value << std::endl;
			// 将数字压入numbers
			numbers.push(value);
			// 更新i和expectNumber
			i += length;
			expectNumber = false;
		}
		else
		{
			char op = expression[i];

			// 处理括号问题
			if (op == '(')
			{
				if (!expectNumber)
				{
					throw std::invalid_argument("Incomplete expression");
				}
				operators.push(op);
				++i;
				expectNumber = true;
				continue;
			}

			if (op == ')')
			{
				if (expectNumber)
				{
					throw std::invalid_argument("Missing operator before '('");
				}
				// 满足条件则计算括号里面的内容
				while (!operators.empty() && operators.top() != '(')
				{
					applyTopOperator(numbers, operators);
				}
				if (operators.empty())
				{
					throw std::invalid_argument("Invalid expression");
				}
				else if(operators.top()=='(')
				{
					operators.pop();
					expectNumber = false;
					++i;
					continue;
				}
			}

			if (op != '+' && op != '-' && op != '*' && op != '/')
			{
				throw std::invalid_argument("Invalid character");
			}
			// 检查运算符是否出现在合法位置
			if (expectNumber)
			{
				throw std::invalid_argument("Missing data");
			}
			// 处理优先级
			while (!operators.empty() && (precedence(operators.top()) >= precedence(op)))
			{
				// 取出栈顶运算符和两个数字进行计算，将结果放回数字栈
				applyTopOperator(numbers, operators);
			}
			//将当前运算符压入operators
			operators.push(op);
			//更新i和expectNumber
			++i;
			expectNumber = true;
		}
	}
	// 检查表达式是否以数字结束
	if (expectNumber)
	{
		throw std::invalid_argument("Incomplete expression");
	}
	// 处理operators中剩余的运算符
	while (!operators.empty())
	{
		if (operators.top() == '(')
		{
			throw std::invalid_argument("Missing ')'");
		}

		applyTopOperator(numbers, operators);
	}
	// 检查numbers最终只剩一个结果
	if (numbers.size() != 1)
	{
		throw std::invalid_argument("Invalid expression");
	}

	// 返回最终结果
	return numbers.top();
}

int main()
{
	while (1)
	{
		// 保存用户输入的表达式
		std::string expression;

		std::cout << "==== Expression Calculator ====" << std::endl;
		std::cout << "Please enter an expression: ";

		// 读取整行输入，可以包括空格
		std::getline(std::cin, expression);

		try
		{
			double result = evaluateExpression(expression);
			std::cout << "result: " << result << std::endl;
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	return 0;
}