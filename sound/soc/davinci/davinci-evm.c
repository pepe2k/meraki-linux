/*
 * ASoC driver for TI DAVINCI EVM platform
 *
 * Author:      Vladimir Barinov, <vbarinov@embeddedalley.com>
 * Copyright:   (C) 2007 MontaVista Software, Inc., <source@mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <asm/dma.h>
#include <asm/mach-types.h>

#ifndef CONFIG_ARCH_TI81XX
#include <mach/asp.h>
#include <mach/edma.h>
#include <mach/mux.h>
#else
#include <plat/asp.h>
#include <asm/hardware/edma.h>
#endif

#include "../codecs/tlv320aic3x.h"
#include "davinci-pcm.h"
#include "davinci-i2s.h"
#include "davinci-mcasp.h"

static inline unsigned int get_audio_format(void)
{
#ifdef CONFIG_MACH_TI8148IPNC
	if(omap_rev() == TI8148_REV_ES1_0)
		return (SND_SOC_DAIFMT_DSP_B | \
		        SND_SOC_DAIFMT_CBM_CFM | SND_SOC_DAIFMT_IB_IF);
	else
#endif
		return (SND_SOC_DAIFMT_DSP_B | \
		        SND_SOC_DAIFMT_CBM_CFM | SND_SOC_DAIFMT_IB_NF);
}

static int evm_hw_params(struct snd_pcm_substream *substream,
			 struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int ret = 0;
	unsigned sysclk;

	/* ASP1 on DM355 EVM is clocked by an external oscillator */
	if (machine_is_davinci_dm355_evm() || machine_is_davinci_dm6467_evm() ||
	    machine_is_davinci_dm365_evm())
		sysclk = 27000000;

	/* ASP0 in DM6446 EVM is clocked by U55, as configured by
	 * board-dm644x-evm.c using GPIOs from U18.  There are six
	 * options; here we "know" we use a 48 KHz sample rate.
	 */
	else if (machine_is_davinci_evm())
		sysclk = 12288000;

	else if (machine_is_davinci_da830_evm() ||
				machine_is_davinci_da850_evm() ||
				machine_is_ti8168evm() ||
				machine_is_ti8148evm() ||
				//machine_is_ti8148ipnc() ||
				machine_is_dm385evm() )
				//machine_is_dm385ipnc())
		sysclk = 24576000;
	else if (machine_is_ti8148ipnc() || machine_is_dm385ipnc())
		sysclk = 24576000;

	/*TI811x use McASP2_AUX 20MHz*/
	else if (machine_is_ti811xevm())
		sysclk = 20000000;

	else
		return -EINVAL;

	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, get_audio_format());
	if (ret < 0)
		return ret;

	/* set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, get_audio_format());
	if (ret < 0)
		return ret;

	/* set the codec system clock */
	ret = snd_soc_dai_set_sysclk(codec_dai, 0, sysclk, SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;

	return 0;
}

static int evm_spdif_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;

	/* set cpu DAI configuration */
	return snd_soc_dai_set_fmt(cpu_dai, get_audio_format());
}

static struct snd_soc_ops evm_ops = {
	.hw_params = evm_hw_params,
};

static struct snd_soc_ops evm_spdif_ops = {
	.hw_params = evm_spdif_hw_params,
};

/* davinci-evm machine dapm widgets */
static const struct snd_soc_dapm_widget aic3x_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_LINE("Line Out", NULL),
	SND_SOC_DAPM_MIC("Mic Jack", NULL),
	SND_SOC_DAPM_LINE("Line In", NULL),
};

/* davinci-evm machine audio_mapnections to the codec pins */
static const struct snd_soc_dapm_route audio_map[] = {
	/* Headphone connected to HPLOUT, HPROUT */
	{"Headphone Jack", NULL, "HPLOUT"},
	{"Headphone Jack", NULL, "HPROUT"},

	/* Line Out connected to LLOUT, RLOUT */
	{"Line Out", NULL, "LLOUT"},
	{"Line Out", NULL, "RLOUT"},
#if defined(CONFIG_MACH_DM385IPNC) || defined(CONFIG_MACH_TI8148IPNC)
	{"LINE1R", NULL, "Mic Bias 2.5V"},
	{"Mic Bias 2.5V", NULL, "Mic Jack"},
#else
	/* Mic connected to (MIC3L | MIC3R) */
	{"MIC3L", NULL, "Mic Bias 2V"},
	{"MIC3R", NULL, "Mic Bias 2V"},
	{"Mic Bias 2V", NULL, "Mic Jack"},

	/* Line In connected to (LINE1L | LINE2L), (LINE1R | LINE2R) */
	{"LINE1L", NULL, "Line In"},
	{"LINE2L", NULL, "Line In"},
	{"LINE1R", NULL, "Line In"},
	{"LINE2R", NULL, "Line In"},
#endif
};

/* Logic for a aic3x as connected on a davinci-evm */
static int evm_aic3x_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;

	/* Add davinci-evm specific widgets */
	snd_soc_dapm_new_controls(codec, aic3x_dapm_widgets,
				  ARRAY_SIZE(aic3x_dapm_widgets));

	/* Set up davinci-evm specific audio path audio_map */
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

	/* not connected */
	snd_soc_dapm_disable_pin(codec, "MONO_LOUT");
	snd_soc_dapm_disable_pin(codec, "HPLCOM");
	snd_soc_dapm_disable_pin(codec, "HPRCOM");

	/* always connected */
	snd_soc_dapm_enable_pin(codec, "Headphone Jack");
	snd_soc_dapm_enable_pin(codec, "Line Out");
	snd_soc_dapm_enable_pin(codec, "Mic Jack");
#if !defined(CONFIG_MACH_DM385IPNC) && !defined(CONFIG_MACH_TI8148IPNC)
	snd_soc_dapm_enable_pin(codec, "Line In");
#endif

	snd_soc_dapm_sync(codec);

	return 0;
}

/* davinci-evm machine dapm widgets */
static const struct snd_soc_dapm_widget aic26_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_MIC("Mic Jack", NULL),
};

/* davinci-evm machine audio_mapnections to the codec pins */
static const struct snd_soc_dapm_route aic26_audio_map[] = {
	/* Headphone connected to HPLOUT, HPROUT */
	{"Headphone Jack", NULL, "LHPOUT"},
	{"Headphone Jack", NULL, "RHPOUT"},

	/* Mic connected to MIC */
	{"MICIN", NULL, "Mic Bias 2V"},
	{"Mic Bias 2V", NULL, "Mic Jack"},
};

/* Logic for a aic3x as connected on a davinci-evm */
static int ipnc_aic26_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;

	/* Add davinci-evm specific widgets */
	snd_soc_dapm_new_controls(codec, aic26_dapm_widgets,
				  ARRAY_SIZE(aic26_dapm_widgets));

	/* Set up davinci-evm specific audio path audio_map */
	snd_soc_dapm_add_routes(codec, aic26_audio_map, ARRAY_SIZE(aic26_audio_map));

	/* not connected */
	snd_soc_dapm_disable_pin(codec, "AUXIN");

	/* always connected */
	snd_soc_dapm_enable_pin(codec, "Headphone Jack");
	snd_soc_dapm_enable_pin(codec, "Mic Jack");

	snd_soc_dapm_sync(codec);
	return 0;
}

/* davinci-evm digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link dm6446_evm_dai = {
	.name = "TLV320AIC3X",
	.stream_name = "AIC3X",
	.cpu_dai_name = "davinci-mcbsp",
	.codec_dai_name = "tlv320aic3x-hifi",
	.codec_name = "tlv320aic3x-codec.1-001b",
	.platform_name = "davinci-pcm-audio",
	.init = evm_aic3x_init,
	.ops = &evm_ops,
};

static struct snd_soc_dai_link dm355_evm_dai = {
	.name = "TLV320AIC3X",
	.stream_name = "AIC3X",
	.cpu_dai_name = "davinci-mcbsp.1",
	.codec_dai_name = "tlv320aic3x-hifi",
	.codec_name = "tlv320aic3x-codec.1-001b",
	.platform_name = "davinci-pcm-audio",
	.init = evm_aic3x_init,
	.ops = &evm_ops,
};

static struct snd_soc_dai_link dm365_evm_dai = {
#ifdef CONFIG_SND_DM365_AIC3X_CODEC
	.name = "TLV320AIC3X",
	.stream_name = "AIC3X",
	.cpu_dai_name = "davinci-mcbsp",
	.codec_dai_name = "tlv320aic3x-hifi",
	.init = evm_aic3x_init,
	.codec_name = "tlv320aic3x-codec.1-0018",
	.ops = &evm_ops,
#elif defined(CONFIG_SND_DM365_VOICE_CODEC)
	.name = "Voice Codec - CQ93VC",
	.stream_name = "CQ93",
	.cpu_dai_name = "davinci-vcif",
	.codec_dai_name = "cq93vc-hifi",
	.codec_name = "cq93vc-codec",
#endif
	.platform_name = "davinci-pcm-audio",
};

static struct snd_soc_dai_link dm6467_evm_dai[] = {
	{
		.name = "TLV320AIC3X",
		.stream_name = "AIC3X",
		.cpu_dai_name= "davinci-mcasp.0",
		.codec_dai_name = "tlv320aic3x-hifi",
		.platform_name ="davinci-pcm-audio",
		.codec_name = "tlv320aic3x-codec.0-001a",
		.init = evm_aic3x_init,
		.ops = &evm_ops,
	},
	{
		.name = "McASP",
		.stream_name = "spdif",
		.cpu_dai_name= "davinci-mcasp.1",
		.codec_dai_name = "dit-hifi",
		.codec_name = "spdif_dit",
		.platform_name = "davinci-pcm-audio",
		.ops = &evm_spdif_ops,
	},
};
static struct snd_soc_dai_link da8xx_evm_dai = {
	.name = "TLV320AIC3X",
	.stream_name = "AIC3X",
	.cpu_dai_name= "davinci-mcasp.0",
	.codec_dai_name = "tlv320aic3x-hifi",
	.codec_name = "tlv320aic3x-codec.0-001a",
	.platform_name = "davinci-pcm-audio",
	.init = evm_aic3x_init,
	.ops = &evm_ops,
};

static struct snd_soc_dai_link ti81xx_evm_dai[] = {
	{
		.name = "TLV320AIC3X",
		.stream_name = "AIC3X",
		.codec_dai_name = "tlv320aic3x-hifi",
		.codec_name = "tlv320aic3x-codec.1-0018",
		.platform_name = "davinci-pcm-audio",
		.init = evm_aic3x_init,
		.ops = &evm_ops,
	},
#ifdef CONFIG_SND_SOC_TI81XX_HDMI
	{
		.name = "HDMI_SOC_LINK",
		.stream_name = "hdmi",
		.cpu_dai_name = "hdmi-dai",
		.platform_name = "davinci-pcm-audio",
		.codec_dai_name = "HDMI-DAI-CODEC",     /* DAI name */
		.codec_name = "hdmi-dummy-codec",
	},
#endif
};

static struct snd_soc_dai_link ti81xx_ipnc_dai[] = {
	{
	.name = "TLV320AIC26",
	.stream_name = "AIC26",
	// Penlin apply kernel patch
	//.cpu_dai_name = "davinci-mcasp.2",
	.cpu_dai_name = "davinci-mcasp.1",
	.codec_dai_name = "tlv320aic26-hifi",
	.codec_name = "tlv320aic26-codec.1-0000",
	.platform_name = "davinci-pcm-audio",
	.init = ipnc_aic26_init,
	.ops = &evm_ops,
	},
#ifdef CONFIG_SND_SOC_TI81XX_HDMI
	{
		.name = "HDMI_SOC_LINK",
		.stream_name = "hdmi",
		.cpu_dai_name = "hdmi-dai",
		.platform_name = "davinci-pcm-audio",
		.codec_dai_name = "HDMI-DAI-CODEC",     /* DAI name */
		.codec_name = "hdmi-dummy-codec",
	},
#endif
};

/* davinci dm6446 evm audio machine driver */
static struct snd_soc_card dm6446_snd_soc_card_evm = {
	.name = "DaVinci DM6446 EVM",
	.dai_link = &dm6446_evm_dai,
	.num_links = 1,
};

/* davinci dm355 evm audio machine driver */
static struct snd_soc_card dm355_snd_soc_card_evm = {
	.name = "DaVinci DM355 EVM",
	.dai_link = &dm355_evm_dai,
	.num_links = 1,
};

/* davinci dm365 evm audio machine driver */
static struct snd_soc_card dm365_snd_soc_card_evm = {
	.name = "DaVinci DM365 EVM",
	.dai_link = &dm365_evm_dai,
	.num_links = 1,
};

/* davinci dm6467 evm audio machine driver */
static struct snd_soc_card dm6467_snd_soc_card_evm = {
	.name = "DaVinci DM6467 EVM",
	.dai_link = dm6467_evm_dai,
	.num_links = ARRAY_SIZE(dm6467_evm_dai),
};

static struct snd_soc_card da830_snd_soc_card = {
	.name = "DA830/OMAP-L137 EVM",
	.dai_link = &da8xx_evm_dai,
	.num_links = 1,
};

static struct snd_soc_card da850_snd_soc_card = {
	.name = "DA850/OMAP-L138 EVM",
	.dai_link = &da8xx_evm_dai,
	.num_links = 1,
};

static struct snd_soc_card ti81xx_snd_soc_card = {
	.name = "TI81XX EVM",
	.dai_link = ti81xx_evm_dai,
	.num_links = ARRAY_SIZE(ti81xx_evm_dai),
};

static struct snd_soc_card ti8148_snd_soc_card = {
	.name = "TI8148 IPNC",
	.dai_link = ti81xx_ipnc_dai,
	.num_links = ARRAY_SIZE(ti81xx_ipnc_dai),
};

static void ti81xx_evm_dai_fixup(void)
{
	if (machine_is_ti8168evm() || machine_is_ti8148evm()
		|| machine_is_ti8148ipnc() ||  machine_is_ti811xevm()) {
		ti81xx_evm_dai[0].cpu_dai_name = "davinci-mcasp.2";
	} else if (machine_is_dm385evm() || machine_is_dm385ipnc()) {
		ti81xx_evm_dai[0].cpu_dai_name = "davinci-mcasp.1";
	} else {
		ti81xx_evm_dai[0].cpu_dai_name = NULL;
	}
}

static struct platform_device *evm_snd_device;
static int __init evm_init(void)
{
	struct snd_soc_card *evm_snd_dev_data;
	int index;
	int ret;

	if (machine_is_davinci_evm()) {
		evm_snd_dev_data = &dm6446_snd_soc_card_evm;
		index = 0;
	} else if (machine_is_davinci_dm355_evm()) {
		evm_snd_dev_data = &dm355_snd_soc_card_evm;
		index = 1;
	} else if (machine_is_davinci_dm365_evm()) {
		evm_snd_dev_data = &dm365_snd_soc_card_evm;
		index = 0;
	} else if (machine_is_davinci_dm6467_evm()) {
		evm_snd_dev_data = &dm6467_snd_soc_card_evm;
		index = 0;
	} else if (machine_is_davinci_da830_evm()) {
		evm_snd_dev_data = &da830_snd_soc_card;
		index = 1;
	} else if (machine_is_davinci_da850_evm()) {
		evm_snd_dev_data = &da850_snd_soc_card;
		index = 0;
	} else if (machine_is_ti8168evm() || machine_is_ti8148evm()
					|| machine_is_dm385evm()
					|| machine_is_dm385ipnc()
                                        || machine_is_ti811xevm()) {
		ti81xx_evm_dai_fixup();
		evm_snd_dev_data = &ti81xx_snd_soc_card;
		index = 0;
	} else if (machine_is_ti8148ipnc()) {
		ti81xx_evm_dai_fixup();
		evm_snd_dev_data = &ti8148_snd_soc_card;
		if(omap_rev() != TI8148_REV_ES1_0)
			evm_snd_dev_data->dai_link = ti81xx_evm_dai;
		index = 0;
	} else
		return -EINVAL;

	evm_snd_device = platform_device_alloc("soc-audio", index);
	if (!evm_snd_device)
		return -ENOMEM;

	platform_set_drvdata(evm_snd_device, evm_snd_dev_data);
	ret = platform_device_add(evm_snd_device);
	if (ret)
		platform_device_put(evm_snd_device);

	return ret;
}

static void __exit evm_exit(void)
{
	platform_device_unregister(evm_snd_device);
}

module_init(evm_init);
module_exit(evm_exit);

MODULE_AUTHOR("Vladimir Barinov");
MODULE_DESCRIPTION("TI DAVINCI EVM ASoC driver");
MODULE_LICENSE("GPL");
