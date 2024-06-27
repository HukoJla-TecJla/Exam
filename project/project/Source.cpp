#include <iostream>
#include <string>

class Person final
{
public:
	Person() = default;
	Person(unsigned age, std::string name) : age{ age }, name{ name } {}

	unsigned getAge() const { return age; }
	std::string getName() const { return name; }

	void setAge(unsigned personAge) { age = personAge; }
	void setName(std::string personName) { name = personName; }

	friend std::ostream& operator <<(std::ostream& os,const Person person)
	{
		return os << person.getAge() << " " << person.getName();
	}

	friend std::istream& operator >>(std::istream& is, Person person)
	{
		unsigned age;
		std::string name;
		is >> age >> name;
		person.setAge(age);
		person.setName(name);
		return is;
	}
	
	Person& operator +=(const Person& right)
	{
		age += right.age;
		return *this;
	}

	Person operator+(Person right) const
	{
		return right += *this;
	}

	Person& operator++()
	{
		age++;
		return *this;
	}

	Person operator++(int)
	{
		Person temp = *this;
		++*this;
		return temp;
	}

	bool operator <(const Person& right) const
	{
		return age < right.age;
	}

	bool operator >(const Person& right) const
	{
		return age > right.age;
	}

	bool operator ==(const Person& right) const
	{
		return age == right.age;
	}

	bool operator !=(const Person& right) const
	{
		return !(*this == right);
	}


private:
	std::string name;
	unsigned age;
};
	 




