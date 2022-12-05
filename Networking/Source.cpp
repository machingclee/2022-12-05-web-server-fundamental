#define ASIO_STANDALONE

#include <iostream>
#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"



#define LOG(x) std::cout << x << std::endl
#define WAIT() std::cin.get()

std::vector<char> vBuffer(20 * 1024);

void GradSomeData(asio::ip::tcp::socket& socket) {
	socket.async_read_some(
		asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length) {
			if (!ec) {
				std::cout << "\n\n Read" << length << " byes \n\n";

				for (int i = 0; i < length; i++) {
					std::cout << vBuffer[i];
				}

				GradSomeData(socket);
			}
		}
	);
}


int main() {
	asio::error_code ec;

	// define platform specific interface:
	asio::io_context context;

	asio::io_context::work idleWork(context);
	std::thread context_thread = std::thread([&]() { context.run(); });


	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("192.168.1.101", ec), 3000);

	// platform specific implementation is delivered to the socket
	asio::ip::tcp::socket socket(context);
	socket.connect(endpoint, ec);

	if (!ec) {
		LOG("connected");
	}
	else {
		LOG("Failed to connect to address:");
		LOG(ec.message());
	}

	if (socket.is_open()) {
		GradSomeData(socket);

		std::string sRequest =
			"GET / HTTP/1.1\r\nHost: localhost:3000\r\nConnection: close\r\n\r\n";
		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(2000ms);

		//size_t bytes = socket.available();
		//std::cout << "Bytes Available: " << bytes << std::endl;

		//if (bytes > 0) {
		//	std::vector<char> vBuffer(bytes);
		//	socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);

		//	for (auto& c : vBuffer) {
		//		std::cout << c;
		//	}
		//}

	}
}