﻿#include "Core/Utilities/QProgInfo/Visualization/Draw.h"
#include "Core/Utilities/QProgInfo/Visualization/CharsTransform.h"
#include "Core/Utilities/Tools/TranformQGateTypeStringAndEnum.h"
#include <iostream>
#include <codecvt>
#include <locale>
#include "QPandaNamespace.h"
#include "Core/Core.h"
#include "Core/QuantumCircuit/QuantumMeasure.h"
#include "Core/QuantumCircuit/QGate.h"
#include "Core/Utilities/Tools/XMLConfigParam.h"

USING_QPANDA
using namespace std;
using namespace QGATE_SPACE;

#define WIRE_HEAD_LEN 6
#define OUTPUT_TMP_FILE ("QCircuitTextPic.txt")

#define BOX_RIGHT_CONNECT_CHAR              0XE2949C     /* UNICODE CHAR:    ├    */
#define BOX_LEFT_CONNECT_CHAR               0XE294A4     /* UNICODE CHAR:    ┤    */
#define BOX_UP_CONNECT_CHAR                 0XE294B4     /* UNICODE CHAR:    ┴    */
#define BOX_DOWN_CONNECT_CHAR               0XE294AC     /* UNICODE CHAR:    ┬    */
#define BOX_LEFT_DOUBLE_CONNECT_CHAR        0XE295A1     /* UNICODE CHAR:    ╡    */
#define BOX_RIGHT_DOUBLE_CONNECT_CHAR       0XE2959E     /* UNICODE CHAR:    ╞    */
#define BOX_DOWN_DOUBLE_CONNECT_CHAR        0XE295A5     /* UNICODE CHAR:    ╥    */
#define BOX_LEFT_TOP_CHAR                   0XE2948C     /* UNICODE CHAR:    ┌    */
#define BOX_RIGHT_TOP_CHAR                  0XE29490     /* UNICODE CHAR:    ┐    */
#define BOX_LEFT_BOTTOM_CHAR                0XE29494     /* UNICODE CHAR:    └    */
#define BOX_RIGHT_BOTTOM_CHAR               0XE29498     /* UNICODE CHAR:    ┘    */

#define SINGLE_VERTICAL_LINE                0XE29482     /* UNICODE CHAR:    │    */
#define DOUBLE_VERTICAL_LINE                0XE29591     /* UNICODE CHAR:    ║    */
#define SINGLE_HORIZONTAL_LINE              0XE29480     /* UNICODE CHAR:    ─    */
#define DOUBLE_HORIZONTAL_LINE              0XE29590     /* UNICODE CHAR:    ═    */
#define CROSS_CHAR                          0XE294BC     /* UNICODE CHAR:    ┼    */
#define DOUBLE_CROSS_CHAR                   0XE295AC     /* UNICODE CHAR:    ╬    */
#define BLACK_SQUARE_CHAR                   0XE296A0     /* UNICODE CHAR:    ■    */  
#define BLACK_PRISMATIC_CHAR                0XE29786     /* UNICODE CHAR:    ◆    */
#define DOUBLE_LINE_UP_CONNECT_CHAR         0XE295A9     /* UNICODE CHAR:    ╩    */
#define SINGLE_LINE_ACROSS_DOUBLE_LINE      0XE295AB     /* UNICODE CHAR:    ╫    */
#define DOUBLE_LINE_ACROSS_SINGLE_LINE      0XE295AA     /* UNICODE CHAR:    ╪    */
#define TIME_SEQUENCE_DIVIDER_LINE          0XE29486     /* UNICODE CHAR:    ╏     */

class MeasureTo : public DrawBox
{
public:
	MeasureTo()
		:DrawBox(
			std::string(" ") + ulongToUtf8(DOUBLE_VERTICAL_LINE) + std::string(" "),
			ulongToUtf8(DOUBLE_HORIZONTAL_LINE) + ulongToUtf8(DOUBLE_LINE_UP_CONNECT_CHAR) + ulongToUtf8(DOUBLE_HORIZONTAL_LINE),
			std::string("   "))
	{}
	~MeasureTo() {}

	int getLen() const { return 3; }

private:
};

class MeasureFrom : public DrawBox
{
public:
	MeasureFrom()
		:DrawBox(
			ulongToUtf8(BOX_LEFT_TOP_CHAR) + ulongToUtf8(SINGLE_HORIZONTAL_LINE) + ulongToUtf8(BOX_RIGHT_TOP_CHAR),
			ulongToUtf8(BOX_LEFT_CONNECT_CHAR) + std::string("M") + ulongToUtf8(BOX_RIGHT_CONNECT_CHAR),
			ulongToUtf8(BOX_LEFT_BOTTOM_CHAR) + ulongToUtf8(BOX_DOWN_DOUBLE_CONNECT_CHAR) + ulongToUtf8(BOX_RIGHT_BOTTOM_CHAR))
	{}
	~MeasureFrom() {}

	int getLen() const { return 3; }

private:
};

class SwapFrom : public DrawBox
{
public:
	SwapFrom()
		:DrawBox(
			std::string(" "),
			std::string("X"),
			ulongToUtf8(SINGLE_VERTICAL_LINE))
	{}

	void set_top_connected() override {
		m_top_format = ulongToUtf8(SINGLE_VERTICAL_LINE);
	}

	void set_bot_connected() override {
		m_bot_format = ulongToUtf8(SINGLE_VERTICAL_LINE);
	}

	int getLen() const { return 1; }

private:
};

class SwapTo : public DrawBox
{
public:
	SwapTo()
		:DrawBox(
			ulongToUtf8(SINGLE_VERTICAL_LINE),
			std::string("X"),
			std::string(" "))
	{}

	void set_top_connected() override {
		m_top_format = ulongToUtf8(SINGLE_VERTICAL_LINE);
	}

	void set_bot_connected() override {
		m_bot_format = ulongToUtf8(SINGLE_VERTICAL_LINE);
	}

	int getLen() const { return 1; }

private:
};

class ControlQuBit : public DrawBox
{
public:
	ControlQuBit()
		:DrawBox(
			std::string(" "),
			ulongToUtf8(BLACK_SQUARE_CHAR),
			std::string(" "))
	{}
	~ControlQuBit() {}

	int getLen() const { return 1; }

	void set_top_connected() override {
		m_top_format = ulongToUtf8(SINGLE_VERTICAL_LINE);
	}

	void set_bot_connected() override {
		m_bot_format = ulongToUtf8(SINGLE_VERTICAL_LINE);
	}

	void set_to_circuit_control() {
		m_mid_format = ulongToUtf8(BLACK_SQUARE_CHAR); //BLACK_PRISMATIC_CHAR
	}

private:
};

class ControlLine : public DrawBox
{
public:
	ControlLine()
		:DrawBox(
			ulongToUtf8(SINGLE_VERTICAL_LINE),
			ulongToUtf8(CROSS_CHAR),
			ulongToUtf8(SINGLE_VERTICAL_LINE))
	{}

	int getLen() const { return 1; }

private:
};

class TimeSequenceLine : public DrawBox
{
#define TIME_SEQUENCE_SEPARATOR_CHAR (":")
public:
	TimeSequenceLine()
		:DrawBox(
			string(TIME_SEQUENCE_SEPARATOR_CHAR),
			string(TIME_SEQUENCE_SEPARATOR_CHAR),
			string(TIME_SEQUENCE_SEPARATOR_CHAR))
	{}

	int getLen() const { return m_len; }

	void set_time_sequence(int val) {
		string val_str = std::to_string(val);
		m_len = val_str.size();
		for (size_t i = 1; i < m_len; ++i)
		{
			m_mid_format.append(ulongToUtf8(SINGLE_HORIZONTAL_LINE));
			m_bot_format.append(" ");
		}
		m_top_format = val_str;
	}

	void reset() {
		m_top_format = TIME_SEQUENCE_SEPARATOR_CHAR;
		m_mid_format = TIME_SEQUENCE_SEPARATOR_CHAR;
		m_bot_format = TIME_SEQUENCE_SEPARATOR_CHAR;
		m_len = 1;
	}

private:
	int m_len{0};
};

class MeasureLine : public DrawBox
{
public:
	MeasureLine(const std::string& mid_format_str)
		:DrawBox(
			ulongToUtf8(DOUBLE_VERTICAL_LINE),
			std::string(mid_format_str),
			ulongToUtf8(DOUBLE_VERTICAL_LINE))
	{}

	static const std::string getMeasureLineCrossClWire() { return ulongToUtf8(DOUBLE_CROSS_CHAR); }
	static const std::string getMeasureLineCrossQuWire() { return ulongToUtf8(SINGLE_LINE_ACROSS_DOUBLE_LINE); }

	int getLen() const { return 1; }

private:
};

class BoxOnWire : public DrawBox
{
public:
	BoxOnWire(const std::string &top_format_str, const std::string &mid_format_str, const std::string &bot_format_str, const std::string &pad_str)
		:DrawBox(
			std::string(top_format_str),
			std::string(mid_format_str),
			std::string(bot_format_str))
		, m_pad_str(pad_str), m_len(0)
	{}

	void setName(const std::string& name) {
		std::string pad_str;
		for (size_t i = 0; i < name.size(); i++)
		{
			pad_str.append(m_pad_str);
		}

		char *buf = new char[name.size() * 3 + 8];

		sprintf(buf, m_top_format.c_str(), pad_str.c_str());
		m_top_format = buf;

		sprintf(buf, m_mid_format.c_str(), name.c_str());
		m_mid_format = buf;

		sprintf(buf, m_bot_format.c_str(), pad_str.c_str());
		m_bot_format = buf;

		m_len = name.size() + 2; // 2 = sizeof("┌┐") / sizeof("┌")
		delete[] buf;
	}

	virtual int getLen() const { return m_len; }

	virtual void setStr(std::string &target_str, const int pos, const std::string &str) {

		int char_start_pos = (pos) * 3; //because the size of utf8-char is 3 Bytes
		for (size_t i = 0; i < str.length(); i++)
		{
			target_str.at(char_start_pos + i) = str.at(i);
		}
	}

protected:
	const std::string m_pad_str;
	int m_len;
};

class BoxOnClWire : public BoxOnWire
{
public:
	BoxOnClWire(const std::string& name)
		:BoxOnWire(
			ulongToUtf8(BOX_LEFT_TOP_CHAR) + std::string("%s") + ulongToUtf8(BOX_RIGHT_TOP_CHAR),
			ulongToUtf8(BOX_LEFT_DOUBLE_CONNECT_CHAR) + std::string("%s") + ulongToUtf8(BOX_RIGHT_DOUBLE_CONNECT_CHAR),
			ulongToUtf8(BOX_LEFT_BOTTOM_CHAR) + std::string("%s") + ulongToUtf8(BOX_RIGHT_BOTTOM_CHAR),
			ulongToUtf8(SINGLE_HORIZONTAL_LINE))
		, m_name(name)
	{
		setName(m_name);
	}
	~BoxOnClWire() {}

private:
	const std::string &m_name;
};

class BoxOnQuWire : public BoxOnWire
{
public:
	BoxOnQuWire(const std::string& name)
		:BoxOnWire(
			ulongToUtf8(BOX_LEFT_TOP_CHAR) + std::string("%s") + ulongToUtf8(BOX_RIGHT_TOP_CHAR),
			ulongToUtf8(BOX_LEFT_CONNECT_CHAR) + std::string("%s") + ulongToUtf8(BOX_RIGHT_CONNECT_CHAR),
			ulongToUtf8(BOX_LEFT_BOTTOM_CHAR) + std::string("%s") + ulongToUtf8(BOX_RIGHT_BOTTOM_CHAR),
			ulongToUtf8(SINGLE_HORIZONTAL_LINE))
		, m_name(name)
		, m_top_connector(ulongToUtf8(BOX_UP_CONNECT_CHAR))
		, m_bot_connector(ulongToUtf8(BOX_DOWN_CONNECT_CHAR))
	{
		setName(m_name);
	}
	~BoxOnQuWire() {}

	void set_top_connected() override {
		int pos = m_len / 2;
		setStr(m_top_format, pos, m_top_connector);
	}

	void set_bot_connected() override {
		int pos = m_len / 2;
		setStr(m_bot_format, pos, m_bot_connector);
	}

private:
	const std::string m_top_connector;
	const std::string m_bot_connector;
	const std::string &m_name;
};

class ClassWire : public Wire
{
public:
	ClassWire()
		:Wire(ulongToUtf8(DOUBLE_HORIZONTAL_LINE))
	{}
	~ClassWire() {}

private:
};

class QuantumWire : public Wire
{
public:
	QuantumWire()
		:Wire(ulongToUtf8(SINGLE_HORIZONTAL_LINE))
	{}
	~QuantumWire() {}

private:
};

DrawPicture::DrawPicture(QProg &prog)
	:m_prog(prog), m_text_len(0), m_max_time_sequence(0), m_time_sequence_conf(TimeSequenceConfig::get_instance())
{}

void DrawPicture::appendMeasure(std::shared_ptr<AbstractQuantumMeasure> pMeasure)
{
	int qubit_index = pMeasure->getQuBit()->getPhysicalQubitPtr()->getQubitAddr();
	int c_bit_index = pMeasure->getCBit()->getValue();

	auto start_quBit = m_quantum_bit_wires.find(qubit_index);
	auto end_quBit = m_quantum_bit_wires.end();
	int append_pos = getMaxQuWireLength(start_quBit, end_quBit);

	MeasureFrom box_measure_from;
	append_pos = start_quBit->second->append(box_measure_from, append_pos);

	update_time_sequence(start_quBit->second, get_measure_time_sequence());

	MeasureTo box_measure_to;
	m_class_bit_wires[c_bit_index]->append(box_measure_to, (append_pos - (box_measure_to.getLen())));

	MeasureLine measure_line_on_qu_wire(MeasureLine::getMeasureLineCrossQuWire());
	int offset = (box_measure_from.getLen() - measure_line_on_qu_wire.getLen()) / 2 + measure_line_on_qu_wire.getLen();
	for (auto itr = ++start_quBit; itr != m_quantum_bit_wires.end(); itr++)
	{
		itr->second->append(measure_line_on_qu_wire, (append_pos - offset));
	}

	MeasureLine measure_line_on_cl_wire(MeasureLine::getMeasureLineCrossClWire());
	offset = (box_measure_from.getLen() - measure_line_on_cl_wire.getLen()) / 2 + measure_line_on_cl_wire.getLen();
	for (size_t i = 0; i < c_bit_index; i++)
	{
		if (m_class_bit_wires.find(i) != m_class_bit_wires.end())
		{
			m_class_bit_wires[i]->append(measure_line_on_cl_wire, (append_pos - (offset)));
		}
	}
}

void DrawPicture::layer()
{
	if (nullptr != m_p_grapth_dag.get())
	{
		m_p_grapth_dag.reset((std::make_shared<GraphMatch>()).get());
	}
	else
	{
		m_p_grapth_dag = std::make_shared<GraphMatch>();
	}

	m_layer_info.clear();
	m_p_grapth_dag->get_topological_sequence(m_prog, m_layer_info);
}

void DrawPicture::append_ctrl_gate(std::string gate_name, const int terget_qubit, QVec &self_control_qubits_vec, QVec &circuit_control_qubits_vec)
{
	// get all the qubits
	std::vector<int> all_qubits;
	all_qubits.push_back(terget_qubit);

	for (auto& itr : self_control_qubits_vec)
	{
		all_qubits.push_back(itr->getPhysicalQubitPtr()->getQubitAddr());
	}

	for (auto& itr : circuit_control_qubits_vec)
	{
		all_qubits.push_back(itr->getPhysicalQubitPtr()->getQubitAddr());
	}

	//sort
	sort(all_qubits.begin(), all_qubits.end(), [](int a, int b) {return a < b; });
	int append_pos = getMaxQuWireLength(m_quantum_bit_wires.find(all_qubits.front()), ++(m_quantum_bit_wires.find(all_qubits.back())));

	//append to text-pic
	BoxOnQuWire quBox(gate_name);
	for (auto itr = all_qubits.begin(); itr != all_qubits.end(); ++itr)
	{
		if (terget_qubit == (*itr))
		{
			//append target qubit
			set_connect_direction(*itr, all_qubits, quBox);
			m_quantum_bit_wires[terget_qubit]->append(quBox, append_pos);
		}
		else
		{
			//append control qubit
			ControlQuBit quControlBox;
			for (auto & itr_cir_ctrl_qubit : circuit_control_qubits_vec)
			{
				if (itr_cir_ctrl_qubit->getPhysicalQubitPtr()->getQubitAddr() == (*itr))
				{
					quControlBox.set_to_circuit_control();
				}
			}
			
			set_connect_direction(*itr, all_qubits, quControlBox);
			m_quantum_bit_wires[*itr]->append(quControlBox, append_pos + (quBox.getLen() / 2));
		}

		// add time sequence
		update_time_sequence(m_quantum_bit_wires[*itr], get_ctrl_node_time_sequence() * (circuit_control_qubits_vec.size() + 1));

		if (itr != all_qubits.begin())
		{
			auto pre_itr = itr - 1;
			append_ctrl_line((*pre_itr) + 1, *itr, append_pos + (quBox.getLen() / 2));
		}
	}
}

void DrawPicture::append_single_gate(std::string gate_name, QVec &qubits_vector, QVec &circuit_control_qubits_vec)
{
	int qubit_index = qubits_vector.front()->getPhysicalQubitPtr()->getQubitAddr();

	// get all the qubits
	std::vector<int> all_qubits;
	all_qubits.push_back(qubit_index);
	for (auto& itr : circuit_control_qubits_vec)
	{
		all_qubits.push_back(itr->getPhysicalQubitPtr()->getQubitAddr());
	}

	//get time sequence
	int time_sequence = circuit_control_qubits_vec.size() > 0 ? (get_ctrl_node_time_sequence() * circuit_control_qubits_vec.size()) : get_single_gate_time_sequence();

	//sort
	sort(all_qubits.begin(), all_qubits.end(), [](int a, int b) {return a < b; });
	int append_pos = getMaxQuWireLength(m_quantum_bit_wires.find(all_qubits.front()), ++(m_quantum_bit_wires.find(all_qubits.back())));

	//append to text-pic
	BoxOnQuWire quBox(gate_name);
	for (auto itr = all_qubits.begin(); itr != all_qubits.end(); ++itr)
	{
		if ((*itr) == qubit_index)
		{
			//append target qubit
			set_connect_direction(*itr, all_qubits, quBox);
			m_quantum_bit_wires[qubit_index]->append(quBox, append_pos);
		}
		else
		{
			//append control qubit
			ControlQuBit quControlBox;
			quControlBox.set_to_circuit_control();
			set_connect_direction(*itr, all_qubits, quControlBox);
			m_quantum_bit_wires[*itr]->append(quControlBox, append_pos + (quBox.getLen() / 2));
		}

		update_time_sequence(m_quantum_bit_wires[*itr], time_sequence);

		if (itr != all_qubits.begin())
		{
			auto pre_itr = itr - 1;
			append_ctrl_line((*pre_itr) + 1, *itr, append_pos + (quBox.getLen() / 2));
		}
	}
}

void DrawPicture::update_time_sequence(std::shared_ptr<Wire> p_wire, int increased_time_sequence)
{
	int cur_wire_time_sequence = p_wire->update_time_sequence(increased_time_sequence);
	if (cur_wire_time_sequence > m_max_time_sequence)
	{
		m_max_time_sequence = cur_wire_time_sequence;
	}
}

void DrawPicture::append_ctrl_line(int line_start, int line_end, int pos)
{
	ControlLine ctr_line;
	for (size_t i = line_start; i < line_end; ++i)
	{
		if (m_quantum_bit_wires.find(i) != m_quantum_bit_wires.end())
		{
			m_quantum_bit_wires[i]->append(ctr_line, pos);
		}
	}
}

void DrawPicture::set_connect_direction(const int& qubit, const std::vector<int>& vec_qubits, DrawBox& box)
{
	if (vec_qubits.size() == 1)
	{
		return;
	}

	if (qubit == vec_qubits.front())
	{
		box.set_bot_connected();
	}
	else if (qubit == vec_qubits.back())
	{
		box.set_top_connected();
	}
	else
	{
		box.set_bot_connected();
		box.set_top_connected();
	}
}

void DrawPicture::append_swap_gate(string gate_name, QVec &qubits_vector, QVec &circuit_control_qubits_vec)
{
	// get all the qubits
	int swap_from_qubit_index = qubits_vector.front()->getPhysicalQubitPtr()->getQubitAddr();
	int swap_to_qubit_index = qubits_vector.back()->getPhysicalQubitPtr()->getQubitAddr();
	std::vector<int> all_qubits;
	all_qubits.push_back(swap_from_qubit_index);
	all_qubits.push_back(swap_to_qubit_index);
	for (auto& itr : circuit_control_qubits_vec)
	{
		all_qubits.push_back(itr->getPhysicalQubitPtr()->getQubitAddr());
	}

	//sort
	sort(all_qubits.begin(), all_qubits.end(), [](int a, int b) {return a < b; });
	int append_pos = getMaxQuWireLength(m_quantum_bit_wires.find(all_qubits.front()), ++(m_quantum_bit_wires.find(all_qubits.back())));

	//append to text-pic
	bool already_append_swap_from_box = false;
	for (auto itr = all_qubits.begin(); itr != all_qubits.end(); ++itr)
	{
		if (((*itr) == swap_from_qubit_index) || ((*itr) == swap_to_qubit_index))
		{
			if (already_append_swap_from_box)
			{
				SwapTo swap_to;
				set_connect_direction(*itr, all_qubits, swap_to);
				m_quantum_bit_wires[(*itr) == swap_from_qubit_index ? swap_from_qubit_index : swap_to_qubit_index]->append(swap_to, append_pos);
			}
			else
			{
				SwapFrom swap_from;
				set_connect_direction(*itr, all_qubits, swap_from);
				m_quantum_bit_wires[(*itr) == swap_from_qubit_index? swap_from_qubit_index: swap_to_qubit_index]->append(swap_from, append_pos);
				already_append_swap_from_box = true;
			}
		}
		else
		{
			//append control qubit
			ControlQuBit quControlBox;
			quControlBox.set_to_circuit_control();
			set_connect_direction(*itr, all_qubits, quControlBox);
			m_quantum_bit_wires[*itr]->append(quControlBox, append_pos);
		}

		// add time sequence
		update_time_sequence(m_quantum_bit_wires[*itr], get_swap_gate_time_sequence() * (circuit_control_qubits_vec.size() + 1));

		if (itr != all_qubits.begin())
		{
			auto pre_itr = itr - 1;
			append_ctrl_line((*pre_itr) + 1, *itr, append_pos);
		}
	}
}



void DrawPicture::append_gate_param(string &gate_name, std::shared_ptr<AbstractQGateNode> p_gate, GateType type)
{
	string gateParamStr;
	get_gate_parameter(p_gate, gateParamStr);
	gate_name = TransformQGateType::getInstance()[type];
	gate_name.append(gateParamStr);
	if (p_gate->isDagger())
	{
		gate_name.append(".dag");
	}
}

void DrawPicture::draw_by_layer()
{
	const QProgDAG &prog_dag = m_p_grapth_dag->getProgDAG();
	for (auto seq_item_itr = m_layer_info.begin(); seq_item_itr != m_layer_info.end(); ++seq_item_itr)
	{
		for (auto &seq_node_item : (*seq_item_itr))
		{
			SequenceNode n = seq_node_item.first;
			if (-1 == n.m_node_type)
			{
				std::shared_ptr<AbstractQuantumMeasure> p_measure = dynamic_pointer_cast<AbstractQuantumMeasure>(prog_dag.get_vertex(n.m_vertex_num));
				appendMeasure(p_measure);
			}
			else
			{
				QVec qubits_vector;
				string gate_name;
				std::shared_ptr<AbstractQGateNode> p_gate = dynamic_pointer_cast<AbstractQGateNode>(prog_dag.get_vertex(n.m_vertex_num));
				p_gate->getQuBitVector(qubits_vector);

				//get control info
				QVec control_qubits_vec;
				p_gate->getControlVector(control_qubits_vec);

				//get gate parameter
				append_gate_param(gate_name, p_gate, (GateType)(n.m_node_type));

				if (1 == qubits_vector.size())
				{
					// single gate
					append_single_gate(gate_name, qubits_vector, control_qubits_vec);
				}
				else if (2 == qubits_vector.size())
				{
					//double gate
					switch ((GateType)(n.m_node_type))
					{
					case ISWAP_THETA_GATE:
					case ISWAP_GATE:
					case SQISWAP_GATE:
					case SWAP_GATE:
						append_swap_gate(gate_name, qubits_vector, control_qubits_vec);
						break;

					case CU_GATE:
					case CNOT_GATE:
					case CZ_GATE:
					case CPHASE_GATE:
					{
						int target_qubit = qubits_vector.back()->getPhysicalQubitPtr()->getQubitAddr();
						qubits_vector.pop_back();
						append_ctrl_gate(gate_name, target_qubit, qubits_vector, control_qubits_vec);
					}
					break;

					default:
						break;
					}

				}
				else
				{
					//other gate type
				}
			}
		}

		//update m_text_len
		updateTextPicLen();
	}

	//merge line
	mergeLine();
}

void DrawPicture::fill_layer(TopologicalSequence::iterator lay_iter)
{
	const QProgDAG &prog_dag = m_p_grapth_dag->getProgDAG();
	QVec vec_qubits_used_in_layer;
	for (auto &seq_node_item : (*lay_iter))
	{
		SequenceNode n = seq_node_item.first;
		if (-1 == n.m_node_type)
		{
			std::shared_ptr<AbstractQuantumMeasure> p_measure = std::dynamic_pointer_cast<AbstractQuantumMeasure>(prog_dag.get_vertex(n.m_vertex_num));
			QMeasure tmp_measure_node(p_measure);
			vec_qubits_used_in_layer.push_back(tmp_measure_node.getQuBit());
		}
		else
		{
			std::shared_ptr<AbstractQGateNode> p_gate = std::dynamic_pointer_cast<AbstractQGateNode>(prog_dag.get_vertex(n.m_vertex_num));
			QGate tmp_gate_node(p_gate);
			QVec gate_qubits;
			tmp_gate_node.getQuBitVector(gate_qubits);
			for (auto &itr : gate_qubits)
			{
				vec_qubits_used_in_layer.push_back(itr);
			}
		}
	}

	std::sort(m_quantum_bits_in_use.begin(), m_quantum_bits_in_use.end());
	std::sort(vec_qubits_used_in_layer.begin(), vec_qubits_used_in_layer.end());

	QVec unused_qubits_vec;
	set_difference(m_quantum_bits_in_use.begin(), m_quantum_bits_in_use.end(), vec_qubits_used_in_layer.begin(), vec_qubits_used_in_layer.end(), std::back_inserter(unused_qubits_vec));

	auto next_iter = lay_iter + 1;
	get_gate_from_next_layer(lay_iter, unused_qubits_vec, next_iter);
}

void DrawPicture::get_gate_from_next_layer(TopologicalSequence::iterator to_fill_lay_iter, QVec &unused_qubits_vec, TopologicalSequence::iterator next_lay_iter)
{
	if ((unused_qubits_vec.size() == 0) || (m_layer_info.end() == next_lay_iter))
	{
		return;
	}

	const QProgDAG &prog_dag = m_p_grapth_dag->getProgDAG();
	auto& target_lay = *to_fill_lay_iter;
	auto& next_lay = *next_lay_iter;
	for (auto seq_node_item_iter = next_lay.begin(); seq_node_item_iter != next_lay.end();)
	{
		bool b_erase_node = false;
		auto& seq_node_item = *seq_node_item_iter;
		SequenceNode n = seq_node_item.first;
		if (-1 == n.m_node_type)
		{
			std::shared_ptr<AbstractQuantumMeasure> p_measure = std::dynamic_pointer_cast<AbstractQuantumMeasure>(prog_dag.get_vertex(n.m_vertex_num));
			QMeasure tmp_measure_node(p_measure);
			for (auto qubit_iter = unused_qubits_vec.begin(); qubit_iter != unused_qubits_vec.end(); ++qubit_iter)
			{
				auto& tmp_qubit = *qubit_iter;
				if (tmp_measure_node.getQuBit() == tmp_qubit)
				{
					target_lay.push_back(seq_node_item);
					unused_qubits_vec.erase(qubit_iter);
					seq_node_item_iter = next_lay.erase(seq_node_item_iter);
					b_erase_node = true;
					break;
				}
			}
		}
		else
		{
			std::shared_ptr<AbstractQGateNode> p_gate = std::dynamic_pointer_cast<AbstractQGateNode>(prog_dag.get_vertex(n.m_vertex_num));
			QGate tmp_gate_node(p_gate);
			QVec gate_qubits;
			tmp_gate_node.getQuBitVector(gate_qubits);
			QVec ctrl_qubits;
			tmp_gate_node.getControlVector(ctrl_qubits);

			gate_qubits.insert(gate_qubits.end(), ctrl_qubits.begin(), ctrl_qubits.end());

			std::sort(gate_qubits.begin(), gate_qubits.end());
			std::sort(unused_qubits_vec.begin(), unused_qubits_vec.end());

			QVec other_gate_qubits_vec;
			set_difference(gate_qubits.begin(), gate_qubits.end(), unused_qubits_vec.begin(), unused_qubits_vec.end(), std::back_inserter(other_gate_qubits_vec));
			if (other_gate_qubits_vec.size() == 0)
			{
				auto node = seq_node_item;
				target_lay.push_back(node);
				seq_node_item_iter = next_lay.erase(seq_node_item_iter);
				b_erase_node = true;
			}
			
			QVec tmp_unused_qubits_vec;
			set_difference(unused_qubits_vec.begin(), unused_qubits_vec.end(), gate_qubits.begin(), gate_qubits.end(), std::back_inserter(tmp_unused_qubits_vec));
			unused_qubits_vec.swap(tmp_unused_qubits_vec);
			if (unused_qubits_vec.size() == 0)
			{
				break;
			}
		}

		if (b_erase_node)
		{
			continue;
		}

		++seq_node_item_iter;
	}

	if (unused_qubits_vec.size() != 0)
	{
		get_gate_from_next_layer(to_fill_lay_iter, unused_qubits_vec, ++next_lay_iter);
	}
}

void DrawPicture::draw_by_time_sequence()
{
	const QProgDAG &prog_dag = m_p_grapth_dag->getProgDAG();
	for (auto seq_item_itr = m_layer_info.begin(); seq_item_itr != m_layer_info.end(); ++seq_item_itr)
	{
		if ((*seq_item_itr).size() == 0)
		{
			continue;
		}

		//fill current layer
		fill_layer(seq_item_itr);

		for (auto &seq_node_item : (*seq_item_itr))
		{
			SequenceNode n = seq_node_item.first;
			if (-1 == n.m_node_type)
			{
				std::shared_ptr<AbstractQuantumMeasure> p_measure = dynamic_pointer_cast<AbstractQuantumMeasure>(prog_dag.get_vertex(n.m_vertex_num));
				appendMeasure(p_measure);
			}
			else
			{
				QVec qubits_vector;
				string gate_name;
				std::shared_ptr<AbstractQGateNode> p_gate = dynamic_pointer_cast<AbstractQGateNode>(prog_dag.get_vertex(n.m_vertex_num));
				p_gate->getQuBitVector(qubits_vector);

				//get control info
				QVec control_qubits_vec;
				p_gate->getControlVector(control_qubits_vec);

				//get gate parameter
				append_gate_param(gate_name, p_gate, (GateType)(n.m_node_type));

				if (1 == qubits_vector.size())
				{
					// single gate
					append_single_gate(gate_name, qubits_vector, control_qubits_vec);
				}
				else if (2 == qubits_vector.size())
				{
					//double gate
					switch ((GateType)(n.m_node_type))
					{
					case ISWAP_THETA_GATE:
					case ISWAP_GATE:
					case SQISWAP_GATE:
					case SWAP_GATE:
						append_swap_gate(gate_name, qubits_vector, control_qubits_vec);
						break;

					case CU_GATE:
					case CNOT_GATE:
					case CZ_GATE:
					case CPHASE_GATE:
					{
						int target_qubit = qubits_vector.back()->getPhysicalQubitPtr()->getQubitAddr();
						qubits_vector.pop_back();
						append_ctrl_gate(gate_name, target_qubit, qubits_vector, control_qubits_vec);
					}
					break;

					default:
						break;
					}

				}
				else
				{
					//other gate type
				}
			}
		}

		// check time sequence
		check_time_sequence(seq_item_itr);

		//update m_text_len
		updateTextPicLen();

		//append time sequence line
		append_time_sequence_line();
	}

	//merge line
	mergeLine();
}

void DrawPicture::append_time_sequence_line()
{
	TimeSequenceLine time_sequence_line;
	int append_pos = getMaxQuWireLength(m_quantum_bit_wires.begin(), m_quantum_bit_wires.end());
	bool is_line_head = true;
	for (auto itr = m_quantum_bit_wires.begin(); itr != m_quantum_bit_wires.end(); ++itr)
	{
		itr->second->update_time_sequence(m_max_time_sequence - itr->second->get_time_sequence());

		if (is_line_head)
		{
			time_sequence_line.set_time_sequence(m_max_time_sequence);
			itr->second->append(time_sequence_line, append_pos);
			time_sequence_line.reset();
			is_line_head = false;
			continue;
		}
		itr->second->append(time_sequence_line, append_pos);
	}
}

void DrawPicture::check_time_sequence(std::vector<SequenceLayer>::iterator cur_layer_iter)
{
	auto next_layer_iter = ++cur_layer_iter;
	if (next_layer_iter == m_layer_info.end())
	{
		return;
	}

	//get the time sequence of each qubit line
	for (auto itr = m_quantum_bit_wires.begin(); itr != m_quantum_bit_wires.end(); ++itr)
	{
		int cur_wire_time_sequence = itr->second->get_time_sequence();
		//if any qubit-line's time sequence is less than others,  try complement by node from next layer
		auto tmp_iter = next_layer_iter;
		while (cur_wire_time_sequence < m_max_time_sequence)
		{
			if (check_time_sequence_one_qubit(itr, tmp_iter))
			{
				++tmp_iter;
				cur_wire_time_sequence = itr->second->get_time_sequence();
			}
			else
			{
				break;
			}
		}
	}
}

bool DrawPicture::check_time_sequence_one_qubit(wireIter qu_wire_itr, std::vector<SequenceLayer>::iterator next_layer_iter)
{
	/*const int cur_wire_time_sequence = qu_wire_itr->second->get_time_sequence();*/

	//if any qubit-line's time sequence is less than others,  try complement by node from next layer
	//if (cur_wire_time_sequence < m_max_time_sequence)
	if (next_layer_iter == m_layer_info.end())
	{
		return false;
	}

	const QProgDAG &prog_dag = m_p_grapth_dag->getProgDAG();
	bool b_could_continue_recurse = true;

	//complement by node from next layer
	auto &node_vec = (*next_layer_iter);
	for (auto seq_node_item = node_vec.begin(); seq_node_item != node_vec.end(); ++seq_node_item)
	{
		const SequenceNode &node = seq_node_item->first;

		if (-1 == node.m_node_type)
		{
			std::shared_ptr<AbstractQuantumMeasure> p_measure = dynamic_pointer_cast<AbstractQuantumMeasure>(prog_dag.get_vertex(node.m_vertex_num));
			if (qu_wire_itr->first == p_measure->getQuBit()->getPhysicalQubitPtr()->getQubitAddr())
			{
				b_could_continue_recurse = append_node_to_cur_time_sequence(qu_wire_itr, seq_node_item, node_vec);
				break;
			}
		}
		else
		{
			QVec control_qubits_vec;
			QVec qubits_vector;
			std::shared_ptr<AbstractQGateNode> p_gate = dynamic_pointer_cast<AbstractQGateNode>(prog_dag.get_vertex(node.m_vertex_num));
			p_gate->getControlVector(control_qubits_vec);
			p_gate->getQuBitVector(qubits_vector);
			if ((qubits_vector.size() > 1) || (control_qubits_vec.size() > 0))
			{
				if ((is_qubit_in_vec(qu_wire_itr->first, control_qubits_vec)) ||
					(is_qubit_in_vec(qu_wire_itr->first, qubits_vector)))
				{
					b_could_continue_recurse = append_node_to_cur_time_sequence(qu_wire_itr, seq_node_item, node_vec);
					break;
				}
			}
			else
			{
				//single gate
				if (qu_wire_itr->first == qubits_vector[0]->getPhysicalQubitPtr()->getQubitAddr())
				{
					b_could_continue_recurse = append_node_to_cur_time_sequence(qu_wire_itr, seq_node_item, node_vec);
					break;
				}
			}
		}
	}

	return b_could_continue_recurse;
}

//if the target seq_node's qubit vector include the target qubit ,return true;
bool DrawPicture::append_node_to_cur_time_sequence(wireIter qu_wire_itr, SequenceLayer::iterator seq_iter, SequenceLayer& node_vec)
{
	const QProgDAG &prog_dag = m_p_grapth_dag->getProgDAG();
	int cur_node_time_sequence = 0;
	const auto &seq_node = seq_iter->first;
	if (-1 == seq_node.m_node_type)
	{
		std::shared_ptr<AbstractQuantumMeasure> p_measure = dynamic_pointer_cast<AbstractQuantumMeasure>(prog_dag.get_vertex(seq_node.m_vertex_num));
		cur_node_time_sequence = get_measure_time_sequence();
		if ((m_max_time_sequence - (qu_wire_itr->second->get_time_sequence())) < cur_node_time_sequence)
		{
			return false;
		}

		appendMeasure(p_measure);
	}
	else
	{
		QVec control_qubits_vec;
		QVec qubits_vector;
		std::shared_ptr<AbstractQGateNode> p_gate = dynamic_pointer_cast<AbstractQGateNode>(prog_dag.get_vertex(seq_node.m_vertex_num));
		p_gate->getControlVector(control_qubits_vec);
		p_gate->getQuBitVector(qubits_vector);

		string gate_name;
		append_gate_param(gate_name, p_gate, (GateType)(seq_node.m_node_type));

		if ((qubits_vector.size() > 1) || (control_qubits_vec.size() > 0))
		{
			if (((m_max_time_sequence - (qu_wire_itr->second->get_time_sequence())) < get_ctrl_node_time_sequence()) ||
				(check_ctrl_gate_time_sequence_conflicting(control_qubits_vec, qubits_vector)))
			{
				return false;
			}

			//double gate
			switch ((GateType)(seq_node.m_node_type))
			{
			case ISWAP_THETA_GATE:
			case ISWAP_GATE:
			case SQISWAP_GATE:
			case SWAP_GATE:
				append_swap_gate(gate_name, qubits_vector, control_qubits_vec);
				break;

			case CU_GATE:
			case CNOT_GATE:
			case CZ_GATE:
			case CPHASE_GATE:
			{
				int target_qubit = qubits_vector.back()->getPhysicalQubitPtr()->getQubitAddr();
				qubits_vector.pop_back();
				append_ctrl_gate(gate_name, target_qubit, qubits_vector, control_qubits_vec);
			}
			break;

			default:
				break;
			}
		}
		else
		{
			//single gate
			append_single_gate(gate_name, qubits_vector, control_qubits_vec);
		}
	}

	node_vec.erase(seq_iter);
	return true;
}

//return true on time_sequence_conflicting, or else return false
bool DrawPicture::check_ctrl_gate_time_sequence_conflicting(const QVec &control_qubits_vec, const QVec &qubits_vector)
{
	const int ctrl_gate_time_sequence = get_ctrl_node_time_sequence();

	auto check_func = [&ctrl_gate_time_sequence, this](Qubit *tmp_qubit_itr){
		int qubit_num = tmp_qubit_itr->getPhysicalQubitPtr()->getQubitAddr();
		auto qu_wire_itr = m_quantum_bit_wires.find(qubit_num);
		if (qu_wire_itr == m_quantum_bit_wires.end())
		{
			QCERR("qubit number is error.");
			throw runtime_error("qubit number is error.");
		}

		if ((m_max_time_sequence - (qu_wire_itr->second->get_time_sequence())) < ctrl_gate_time_sequence)
		{
			return true;
		}

		return false;
	};

	for (const auto& qubit_itr : control_qubits_vec)
	{
		if (check_func(qubit_itr))
		{
			return true;
		}
	}

	for (const auto& qubit_itr : qubits_vector)
	{
		if (check_func(qubit_itr))
		{
			return true;
		}
	}

	return false;
}

bool DrawPicture::is_qubit_in_vec(const int qubit, const QVec& vec)
{
	for (auto& qubit_itr : vec)
	{
		if (qubit == qubit_itr->getPhysicalQubitPtr()->getQubitAddr())
		{
			return true;
		}
	}

	return false;
}

int DrawPicture::getMaxQuWireLength(wireIter start_quBit_wire, wireIter end_quBit_wire)
{
	int max_length = -1;
	int tmp_length = 0;
	for (auto itr = start_quBit_wire; itr != end_quBit_wire; ++itr)
	{
		tmp_length = itr->second->getWireLength();
		if (tmp_length > max_length)
		{
			max_length = tmp_length;
		}
	}

	return max_length;
}

void DrawPicture::updateTextPicLen()
{
	auto max_len = getMaxQuWireLength(m_quantum_bit_wires.begin(), m_quantum_bit_wires.end());
	for (auto &itr : m_quantum_bit_wires)
	{
		itr.second->updateWireLen(max_len);
	}

	m_text_len = max_len;
}

int DrawPicture::get_wide_char_pos(const std::string &str, int start_pos)
{
	for (size_t i = start_pos; i < str.length(); i++)
	{
		if (((str.at(i) == (char)(0xE2)) && (str.at(i + 1) == (char)(0x96)) && (str.at(i + 2) == (char)(0xA0))) ||
			((str.at(i) == (char)(0xE2)) && (str.at(i + 1) == (char)(0x97)) && (str.at(i + 2) == (char)(0x86))))
		{
			return i;
		}
	}

	return -1;
}

void DrawPicture::fit_to_gbk(std::string &utf8_str)
{
	int pos = 0;
	while (true)
	{
		pos = get_wide_char_pos(utf8_str, pos);
		if (0 > pos)
		{
			break;
		}

		utf8_str.erase(pos + 3, 1);
		utf8_str.erase(pos + 3, 1);
		utf8_str.erase(pos + 3, 1);
		pos += 3;
	}
}

string DrawPicture::present()
{
	/* write to file */
	ofstream outfile(OUTPUT_TMP_FILE, ios::out | ios::binary);
	if (!outfile.is_open())
	{
		throw runtime_error("Can NOT open the output file");
	}

	std::string outputStr = "\n";
	for (auto &itr : m_quantum_bit_wires)
	{
		outputStr.append(itr.second->draw(outfile, 0));
	}

	for (auto &itr : m_class_bit_wires)
	{
		outputStr.append(itr.second->draw(outfile, 0));
	}
	outfile.close();
	
#ifdef _MSC_VER
	fit_to_gbk(outputStr);
	//outputStr = Utf8ToGbkOnWin32(outputStr.c_str());
	outputStr = utf8ToGbk(outputStr);
#endif

	return outputStr;
}

void DrawPicture::init(std::vector<int>& quBits, std::vector<int>& clBits)
{
	const std::string quantum_wire_pad = string("|0>") + ulongToUtf8(SINGLE_HORIZONTAL_LINE);
	const std::string class_wire_pad = string(" 0 ") + ulongToUtf8(DOUBLE_HORIZONTAL_LINE);
	char head_buf[WIRE_HEAD_LEN + 2] = "";
	for (auto i : quBits)
	{
		auto p = std::make_shared<QuantumWire>();
		sprintf(head_buf, "q_%d:", i);
		for (size_t j = strlen(head_buf); j < WIRE_HEAD_LEN; j++)
		{
			head_buf[j] = ' ';
		}

		string name = string(head_buf) + quantum_wire_pad;
		p->setName(name, name.size() - 2); //because the size of utf8-char is 3 Bytes
		m_quantum_bit_wires.insert(wireElement(i, p));
	}

	memset(head_buf, 0, sizeof(head_buf));
	for (auto i : clBits)
	{
		auto p = std::make_shared<ClassWire>();
		sprintf(head_buf, " c_%d:", i);
		for (size_t j = strlen(head_buf); j < WIRE_HEAD_LEN; j++)
		{
			head_buf[j] = ' ';
		}

		string name = string(head_buf) + class_wire_pad;
		p->setName(name, name.size() - 2);//because the size of utf8-char is 3 Bytes
		m_class_bit_wires.insert(wireElement(i, p));
	}

	m_text_len = m_quantum_bit_wires.begin()->second->getWireLength();

	get_all_used_qubits(m_prog, m_quantum_bits_in_use);

	//layer
	layer();
}

void DrawPicture::mergeLine()
{
	std::shared_ptr<Wire> upside_wire = m_quantum_bit_wires.begin()->second;
	for (auto downside_wire = ++(m_quantum_bit_wires.begin()); downside_wire != m_quantum_bit_wires.end(); ++downside_wire)
	{
		merge(upside_wire->getBotLine(), const_cast<std::string&>((downside_wire->second)->getTopLine()));
		upside_wire->setMergedFlag(true);
		upside_wire = (downside_wire->second);
	}

	for (auto downside_wire = m_class_bit_wires.begin(); downside_wire != m_class_bit_wires.end(); ++downside_wire)
	{
		merge(upside_wire->getBotLine(), const_cast<std::string&>((downside_wire->second)->getTopLine()));
		upside_wire->setMergedFlag(true);
		upside_wire = (downside_wire->second);
	}
}

void DrawPicture::merge(const std::string& up_wire, std::string& down_wire)
{
	const char* p_upside_str = up_wire.c_str();
	const char* p_downside_str = (down_wire.c_str());
	unsigned long upside_wide_char = 0;
	unsigned long downside_wide_char = 0;
	std::string tmp_str;
	char wide_char_buf[4] = "";
	size_t upside_char_index = 0;
	size_t downside_char_index = 0;

	for (size_t upside_char_index = 0; upside_char_index < up_wire.size(); ++upside_char_index, ++downside_char_index)
	{
		if (down_wire.size() == downside_char_index)
		{
			tmp_str.append(p_upside_str + upside_char_index);
			break;
		}

		if ((p_upside_str[upside_char_index] == ' ') && (' ' == p_downside_str[downside_char_index]))
		{
			tmp_str.append(" ");
		}
		else if ((p_upside_str[upside_char_index] == ':') && 
			((':' == p_downside_str[downside_char_index]) || (' ' == p_downside_str[downside_char_index])))
		{
			tmp_str.append(TIME_SEQUENCE_SEPARATOR_CHAR);
		}
		else if (p_upside_str[upside_char_index] == ' ')
		{
			wide_char_buf[0] = p_downside_str[downside_char_index];
			wide_char_buf[1] = p_downside_str[++downside_char_index];
			wide_char_buf[2] = p_downside_str[++downside_char_index];
			tmp_str.append(wide_char_buf);
			continue;
		}
		else if (' ' == p_downside_str[downside_char_index])
		{
			wide_char_buf[0] = p_upside_str[upside_char_index];
			wide_char_buf[1] = p_upside_str[++upside_char_index];
			wide_char_buf[2] = p_upside_str[++upside_char_index];
			tmp_str.append(wide_char_buf);
			continue;
		}
		else
		{
			upside_wide_char = getWideCharVal((unsigned char*)p_upside_str + upside_char_index);
			downside_wide_char = getWideCharVal((unsigned char*)p_downside_str + downside_char_index);

			if (upside_wide_char == downside_wide_char)
			{
				wide_char_buf[0] = p_downside_str[downside_char_index];
				wide_char_buf[1] = p_downside_str[++downside_char_index];
				wide_char_buf[2] = p_downside_str[++downside_char_index];
				upside_char_index += 2;
				tmp_str.append(wide_char_buf);
				continue;
			}

			const size_t old_size = tmp_str.size();
			switch (upside_wide_char)
			{
			case BOX_LEFT_BOTTOM_CHAR:
			{
				if (BOX_LEFT_TOP_CHAR == downside_wide_char)
				{
					tmp_str.append(ulongToUtf8(BOX_RIGHT_CONNECT_CHAR));
				}
				else if (BOX_RIGHT_TOP_CHAR == downside_wide_char)
				{
					tmp_str.append(ulongToUtf8(CROSS_CHAR));
				}
				else if (SINGLE_HORIZONTAL_LINE == downside_wide_char)
				{
					tmp_str.append(ulongToUtf8(BOX_UP_CONNECT_CHAR));
				}
			}
			break;

			case BOX_RIGHT_BOTTOM_CHAR:
			{
				if (BOX_RIGHT_TOP_CHAR == downside_wide_char)
				{
					tmp_str.append(ulongToUtf8(BOX_LEFT_CONNECT_CHAR));
				}
				else if (BOX_LEFT_TOP_CHAR == downside_wide_char)
				{
					tmp_str.append(ulongToUtf8(CROSS_CHAR));
				}
				else if (SINGLE_HORIZONTAL_LINE == downside_wide_char)
				{
					tmp_str.append(ulongToUtf8(BOX_UP_CONNECT_CHAR));
				}
			}
			break;

			case SINGLE_VERTICAL_LINE:
			{
				if (BOX_UP_CONNECT_CHAR == downside_wide_char)
				{
					tmp_str.append(ulongToUtf8(BOX_UP_CONNECT_CHAR));
				}
			}
			break;

			case BOX_DOWN_CONNECT_CHAR:
			{
				if (SINGLE_VERTICAL_LINE == downside_wide_char)
				{
					tmp_str.append(ulongToUtf8(BOX_DOWN_CONNECT_CHAR));
				}
			}
			break;

			case SINGLE_HORIZONTAL_LINE:
			{
				if ((BOX_RIGHT_TOP_CHAR == downside_wide_char) || (BOX_LEFT_TOP_CHAR == downside_wide_char))
				{
					tmp_str.append(ulongToUtf8(BOX_DOWN_CONNECT_CHAR));
				}
			}
			break;

			case BOX_DOWN_DOUBLE_CONNECT_CHAR:
			{
				if (DOUBLE_VERTICAL_LINE == downside_wide_char)
				{
					tmp_str.append(ulongToUtf8(BOX_DOWN_DOUBLE_CONNECT_CHAR));
				}
			}
			break;

			default:
				break;
			}

			if (old_size == tmp_str.size())
			{
				tmp_str.append("@"); //This symbol indicates that an error occurred
			}

			upside_char_index += 2;
			downside_char_index += 2;
		}
	}

	if (down_wire.size() > downside_char_index)
	{
		tmp_str.append(p_downside_str + downside_char_index);
	}

	down_wire = tmp_str;
}

int DrawPicture::get_measure_time_sequence()
{
	return m_time_sequence_conf.get_measure_time_sequence();
}

int DrawPicture::get_ctrl_node_time_sequence() 
{
	return m_time_sequence_conf.get_ctrl_node_time_sequence();
}

int DrawPicture::get_swap_gate_time_sequence()
{
	return m_time_sequence_conf.get_swap_gate_time_sequence();
}

int DrawPicture::get_single_gate_time_sequence()
{
	return m_time_sequence_conf.get_single_gate_time_sequence();
}