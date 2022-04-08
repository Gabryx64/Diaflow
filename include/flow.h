#pragma once
#include<string>
#include<vector>
#include<unordered_map>

// TinyXML2
#include<tinyxml2.h>

namespace Diaflow
{
	class Block
	{
	public:
		virtual void xml(tinyxml2::XMLElement* parent) = 0;
		
		virtual ~Block() = default;
	};

	class Comp : public Block, public std::vector<Block*>
	{
	public:
		using std::vector<Block*>::vector;

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("comp");
			for(auto& b : *this)
			{
				b->xml(element);
			}
		}
	};

	class Program
	{
	public:
		std::unordered_map<std::string, Comp> funcs;

		Program()
		{}

		Comp& operator[](const std::string& func)
		{
			if(funcs.find(func) == funcs.end())
				funcs[func] = Comp();

			return funcs[func];
		}

		std::string to_string()
		{
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLElement* root = doc.NewElement("prog");
			doc.InsertEndChild(root);
			
			for(auto& [funcName, funcBody] : funcs)
			{
				tinyxml2::XMLElement* func = root->InsertNewChildElement("func");
				func->SetAttribute("name", funcName.c_str());
				funcBody.xml(func);
			}

			tinyxml2::XMLPrinter printer;
			doc.Print(&printer);
			return printer.CStr();
		}
	};

	class If : public Block
	{
	public:
		std::string cond;
		Comp t, f;

		If(const std::string& cond, const Comp& t, const Comp& f)
			: cond(cond), t(t), f(f)
		{}

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("if");
			element->SetAttribute("cond", cond.c_str());
			t.xml(element);
			f.xml(element);
		}

		~If()
		{
			for(Block* block : t)
				delete block;
			for(Block* block : f)
				delete block;
		}
	};

	class While : public Block
	{
	public:
		std::string cond;
		Comp body;

		While(const std::string& cond, const Comp& body)
			: cond(cond), body(body)
		{}

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("while");
			element->SetAttribute("cond", cond.c_str());
			body.xml(element);
		}

		~While()
		{
			for(Block* block : body)
				delete block;
		}
	};

	class For : public Block
	{
	public:
		std::string init, cond, inc;
		Comp body;

		For(const std::string& init, const std::string& cond, const std::string& inc, const Comp& body)
			: init(init), cond(cond), inc(inc), body(body)
		{}

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("for");
			element->SetAttribute("init", init.c_str());
			element->SetAttribute("cond", cond.c_str());
			element->SetAttribute("inc", inc.c_str());
			body.xml(element);
		}

		~For()
		{
			for(Block* block : body)
				delete block;
		}
	};

	class Foreach : public Block
	{
	public:
		std::string var, iter;
		Comp body;

		Foreach(const std::string& var, const std::string& iter, const Comp& body)
			: var(var), iter(iter), body(body)
		{}

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("foreach");
			element->SetAttribute("var", var.c_str());
			element->SetAttribute("iter", iter.c_str());
			body.xml(element);
		}

		~Foreach()
		{
			for(Block* block : body)
				delete block;
		}
	};

	typedef std::pair<std::string, Comp> Case;
    typedef std::vector<Case> Cases;

	class Switch : public Block
	{
	public:
		std::string expr;
        Cases cases;

		Switch(const std::string& expr, const Cases& cases)
			: expr(expr), cases(cases)
		{}

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("switch");
			element->SetAttribute("expr", expr.c_str());
			for(auto& [caseExpr, caseBody] : cases)
			{
				tinyxml2::XMLElement* case_element = element->InsertNewChildElement("case");
				case_element->SetAttribute("expr", caseExpr.c_str());
				caseBody.xml(case_element);
			}
		}

		~Switch()
		{
			for(auto [caseExpr, caseBody] : cases)
			{
				for(Block* block : caseBody)
					delete block;
			}
		}
	};
}

