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

	typedef std::vector<Block*> Comp;	
	typedef std::vector<std::string> Args;
	
	class Assign : public Block
	{
	public:
		std::string expr;

		Assign(const std::string& expr)
			: expr(expr)
		{}

		void xml(tinyxml2::XMLElement* parent) override
		{
			tinyxml2::XMLElement* assign = parent->InsertNewChildElement("assign");
			assign->SetAttribute("expr", expr.c_str());
		}
	};

	class Input : public Block
	{
	public:
		std::string expr;

		Input(const std::string& expr)
			: expr(expr)
		{}

		void xml(tinyxml2::XMLElement* parent) override
		{
			tinyxml2::XMLElement* input = parent->InsertNewChildElement("in");
			input->SetAttribute("expr", expr.c_str());
		}
	};

	class Output : public Block
	{
	public:
		std::string expr;
		bool newline;

		Output(const std::string& expr, bool newline = true)
			: expr(expr), newline(newline)
		{}

		void xml(tinyxml2::XMLElement* parent) override
		{
			tinyxml2::XMLElement* output = parent->InsertNewChildElement(newline ? "out" : "outln");
			output->SetAttribute("expr", expr.c_str());
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
			
			tinyxml2::XMLElement* then_element = element->InsertNewChildElement("then");
			for(Block* block : t)
				block->xml(then_element);

			tinyxml2::XMLElement* else_element = element->InsertNewChildElement("else");
			for(Block* block : f)
				block->xml(else_element);
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
			
			for(Block* block : body)
				block->xml(element);
		}

		~While()
		{
			for(Block* block : body)
				delete block;
		}
	};

	class DoWhile : public Block
	{
	public:
		std::string cond;
		Comp body;

		DoWhile(const std::string& cond, const Comp& body)
			: cond(cond), body(body)
		{}

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("dowhile");
			element->SetAttribute("cond", cond.c_str());
			
			for(Block* block : body)
				block->xml(element);
		}

		~DoWhile()
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
			
			for(Block* block : body)
				block->xml(element);
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

			for(Block* block : body)
				block->xml(element);
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
			
			for(auto& [expr, body] : cases)
			{
				tinyxml2::XMLElement* case_element = element->InsertNewChildElement("case");
				case_element->SetAttribute("expr", expr.c_str());
				
				for(Block* block : body)
					block->xml(case_element);
			}
		}

		~Switch()
		{
			for(auto [_, body] : cases)
			{
				for(Block* block : body)
					delete block;
			}
		}
	};

	class Break : public Block
	{
	public:
		inline void xml(tinyxml2::XMLElement* parent)
		{
			parent->InsertNewChildElement("break");
		}
	};

	class Continue : public Block
	{
	public:
		inline void xml(tinyxml2::XMLElement* parent)
		{
			parent->InsertNewChildElement("continue");
		}
	};

	class Call : public Block
	{
	public:
		std::string name;
		std::vector<std::string> args;
		std::string retvar;

		Call(const std::string& name, const std::vector<std::string>& args, const std::string& retvar)
			: name(name), args(args), retvar(retvar)
		{}

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("call");
			element->SetAttribute("name", name.c_str());
			element->SetAttribute("retvar", retvar.c_str());

			for(const std::string& arg : args)
			{
				tinyxml2::XMLElement* arg_element = element->InsertNewChildElement("arg");
				arg_element->SetAttribute("expr", arg.c_str());
			}
		}
	};

	class Return : public Block
	{
	public:
		std::string expr;

		Return(const std::string& expr)
			: expr(expr)
		{}

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("return");
			element->SetAttribute("expr", expr.c_str());
		}
	};

	class Comment : public Block
	{
	public:
		std::string comment;

		Comment(const std::string& comment)
			: comment(comment)
		{}

		inline void xml(tinyxml2::XMLElement* parent)
		{
			tinyxml2::XMLElement* element = parent->InsertNewChildElement("comment");
			element->SetAttribute("comment", comment.c_str());
		}
	};

	class Program
	{
	public:
		std::unordered_map<std::string, std::pair<Args, Comp>> funcs;

		Program()
		{}

		Program(const std::string& filename, bool* corrupted = nullptr)
		{
			tinyxml2::XMLDocument doc;
			doc.LoadFile(filename.c_str());
			
			tinyxml2::XMLElement* root = doc.FirstChildElement("prog");
			if (root == nullptr)
			{
				if(corrupted)
					*corrupted = true;
				return;
			}

			for(tinyxml2::XMLElement* func = root->FirstChildElement("func"); func != nullptr; func = func->NextSiblingElement("func"))
			{
				std::string name = func->Attribute("name");
				Args args;

				for(tinyxml2::XMLElement* arg = func->FirstChildElement("arg"); arg != nullptr; arg = arg->NextSiblingElement("arg"))
					args.push_back(arg->Attribute("name"));

				Comp body;
				for(tinyxml2::XMLElement* element = func->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
				{
					Block* block = Program::parse(element);
					if(!block)
					{
						if(corrupted)
							*corrupted = true;

						for(auto& [_, func] : funcs)
						{
							for(Block* block : func.second)
								delete block;
						}

						for(Block* block : body)
							delete block;

						return;
					}

					body.push_back(block);
				};
			}
		}

		static Block* parse(tinyxml2::XMLElement* element)
		{						
			std::string type = element->Name();
			if(type == "assign")
			{
				const char* expr = element->Attribute("expr");
				if(!expr)
					return nullptr;

				return new Assign(expr);
			}

			if(type == "in")
			{
				const char* expr = element->Attribute("expr");
				if(!expr)
					return nullptr;

				return new Input(expr);
			}

			if(type == "outln")
			{
				const char* expr = element->Attribute("expr");
				if(!expr)
					return nullptr;

				return new Output(expr);
			}

			if(type == "out")
			{
				const char* expr = element->Attribute("expr");
				if(!expr)
					return nullptr;

				return new Output(expr, false);
			}

			if(type == "if")
			{
				const char* cond = element->Attribute("cond");
				if(!cond)
					return nullptr;

				tinyxml2::XMLElement* then_element = element->FirstChildElement("then");
				if(!then_element)
					return nullptr;

				tinyxml2::XMLElement* else_element = element->FirstChildElement("else");
				if(!else_element)
					return nullptr;

				Comp then_body;
				for(tinyxml2::XMLElement* element = then_element->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
				{
					Block* block = Program::parse(element);
					if(!block)
					{
						for(Block* block : then_body)
							delete block;

						return nullptr;
					}

					then_body.push_back(block);
				}

				Comp else_body;
				for(tinyxml2::XMLElement* element = else_element->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
				{
					Block* block = Program::parse(element);
					if(!block)
					{
						for(Block* block : then_body)
							delete block;

						for(Block* block : else_body)
							delete block;

						return nullptr;
					}

					else_body.push_back(block);
				}

				return new If(cond, then_body, else_body);
			}

			if(type == "while")
			{
				const char* cond = element->Attribute("cond");
				if(!cond)
					return nullptr;

				Comp body;
				for(tinyxml2::XMLElement* child_element = element->FirstChildElement(); child_element != nullptr; child_element = child_element->NextSiblingElement())
				{
					Block* block = Program::parse(element);
					if(!block)
					{
						for(Block* block : body)
							delete block;

						return nullptr;
					}

					body.push_back(block);
				}

				return new While(cond, body);
			}

			if(type == "dowhile")
			{
				const char* cond = element->Attribute("cond");
				if(!cond)
					return nullptr;

				Comp body;
				for(tinyxml2::XMLElement* child_element = element->FirstChildElement(); child_element != nullptr; child_element = child_element->NextSiblingElement())
				{
					Block* block = Program::parse(element);
					if(!block)
					{
						for(Block* block : body)
							delete block;

						return nullptr;
					}

					body.push_back(block);
				}

				return new DoWhile(cond, body);
			}

			if(type == "for")
			{
				const char* init = element->Attribute("init");
				if(!init)
					return nullptr;

				const char* cond = element->Attribute("cond");
				if(!cond)
					return nullptr;

				const char* inc = element->Attribute("inc");
				if(!inc)
					return nullptr;

				Comp body;
				for(tinyxml2::XMLElement* child_element = element->FirstChildElement(); child_element != nullptr; child_element = child_element->NextSiblingElement())
				{
					Block* block = Program::parse(element);
					if(!block)
					{
						for(Block* block : body)
							delete block;

						return nullptr;
					}

					body.push_back(block);
				}

				return new For(init, cond, inc, body);
			}

			if(type == "foreach")
			{
				const char* var = element->Attribute("var");
				if(!var)
					return nullptr;

				const char* iter = element->Attribute("iter");
				if(!iter)
					return nullptr;

				Comp body;
				for(tinyxml2::XMLElement* child_element = element->FirstChildElement(); child_element != nullptr; child_element = child_element->NextSiblingElement())
				{
					Block* block = Program::parse(element);
					if(!block)
					{
						for(Block* block : body)
							delete block;

						return nullptr;
					}

					body.push_back(block);
				}

				return new Foreach(var, iter, body);
			}

			if(type == "switch")
			{
				const char* expr = element->Attribute("expr");
				if(!expr)
					return nullptr;

				Cases cases;
				for(tinyxml2::XMLElement* child_element = element->FirstChildElement(); child_element != nullptr; child_element = child_element->NextSiblingElement())
				{
					if(std::string(child_element->Name()) != "case")
						continue;

					Comp body;
					for(tinyxml2::XMLElement* element = child_element->FirstChildElement(); element != nullptr; element = element->NextSiblingElement())
					{
						Block* block = Program::parse(element);
						if(!block)
						{
							for(auto& [_, body] : cases)
							{
								for(Block* block : body)
									delete block;
							}

							for(Block* block : body)
								delete block;

							return nullptr;
						}

						body.push_back(block);
					}

					const char* case_expr = child_element->Attribute("expr");
					if(!case_expr)
					{
						for(auto& [_, body] : cases)
						{
							for(Block* block : body)
								delete block;
						}

						for(Block* block : body)
							delete block;

						return nullptr;
					}

					cases.push_back(std::make_pair(case_expr, body));
				}
			}

			if(type == "break")
				return new Break();

			if(type == "continue")
				return new Continue();

			if(type == "call")
			{
				const char* name = element->Attribute("name");
				const char* retvar = element->Attribute("retvar");

				std::vector<std::string> args;
				for(tinyxml2::XMLElement* child_element = element->FirstChildElement(); child_element != nullptr; child_element = child_element->NextSiblingElement())
				{
					if(std::string(child_element->Name()) != "arg")
						return nullptr;

					const char* expr = child_element->Attribute("expr");
					if(!expr)
						return nullptr;

					args.push_back(expr);
				}

				return new Call(name, args, retvar);
			}

			if(type == "return")
			{
				const char* expr = element->Attribute("expr");
				if(!expr)
					return nullptr;

				return new Return(expr);
			}

			if(type == "comment")
			{
				const char* comment = element->Attribute("comment");
				if(!comment)
					return nullptr;

				return new Comment(comment);
			}

			return nullptr;
		}

		std::pair<Args, Comp>& operator[](const std::string& func)
		{
			if(funcs.find(func) == funcs.end())
				funcs[func] = std::make_pair(Args(), Comp());

			return funcs[func];
		}

		tinyxml2::XMLDocument xml()
		{
			tinyxml2::XMLDocument doc;
			tinyxml2::XMLElement* root = doc.NewElement("prog");
			doc.InsertEndChild(root);

			for(auto& [name, func] : funcs)
			{
				auto& [args, body] = func;

				tinyxml2::XMLElement* element = root->InsertNewChildElement("func");
				element->SetAttribute("name", name.c_str());

				for(std::string arg : args)
					element->InsertNewChildElement("arg")->SetAttribute("name", arg.c_str());

				tinyxml2::XMLElement* body_element = element->InsertNewChildElement("body");
				for(Block* block : body)
					block->xml(body_element);
			}

			return doc.ToDocument();
		}

		std::string xml_string()
		{
			tinyxml2::XMLDocument doc = xml();
			tinyxml2::XMLPrinter printer;
			doc.Print(&printer);
			return printer.CStr();
		}	
	};

}

