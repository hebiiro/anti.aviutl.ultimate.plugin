#pragma once

namespace apn::audio_visualizer
{
	std::vector<std::complex<double>> FFT(const std::vector<double>& data)
	{
		//虚数単位
		const std::complex<double>Imaginary(0, 1);

		//累乗を求める
		unsigned int exponent = int(std::ceil(std::log2(data.size())));

		//2の累乗の数
		unsigned int num = int(std::pow(2.0, exponent));

		//計算用バッファ
		std::vector<std::complex<double>>buf(num, 0);

		//ビット反転
		std::vector<unsigned int>index(buf.size());
		for (size_t i = 0; i < index.size(); ++i)
		{
			auto tmp = (unsigned int)i;
			for (unsigned int n = 0; n < exponent; ++n)
			{
				index[i] <<= 1;
				index[i] |= (tmp >> n) & 0x0001;
			}
			//渡されたデータの並びを変える
			if (index[i] < data.size())
			{
				buf[i] = data[index[i]];
			}
			else
			{
				//データ数を超えたら0埋め
				buf[i] = 0.0;
			}
		}

		//バタフライ演算
		for (unsigned int stage = 1; stage <= exponent; ++stage)
		{
			for (unsigned int i = 0; i < std::pow(2.0, exponent - stage); ++i)
			{
				for (unsigned int n = 0; n < std::pow(2.0, stage - 1); ++n)
				{
					std::complex<double> corre1 = std::exp(-Imaginary * 2.0 * std::acos(-1.0) * double(n) / std::pow(2.0, stage));
					std::complex<double> corre2 = std::exp(-Imaginary * 2.0 * std::acos(-1.0) * double(n + std::pow(2.0, stage - 1)) / std::pow(2.0, stage));

					unsigned int No1 = int(i * std::pow(2.0, stage) + n);
					unsigned int No2 = int(No1 + std::pow(2.0, stage - 1));

					std::complex<double> tmp1 = buf[No1];
					std::complex<double> tmp2 = buf[No2];

					buf[No1] = tmp1 + (tmp2 * corre1);
					buf[No2] = tmp1 + (tmp2 * corre2);
				}
			}
		}

		return buf;
	}
}
