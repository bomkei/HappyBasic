import logging
import discord
import subprocess
from lib import process_output
from discord.ext import commands


class msg(commands.Cog):

    def __init__(self, bot):
        self.bot = bot
        self.logger = logging.getLogger(__name__)

    @commands.command(aliases=["bb"])
    async def buildbasic(self, ctx):
        msg = "BUILD FROM BASIC LANG IN C++\n"
        m: discord.Message = await ctx.send(msg)
        p = subprocess.Popen(["make","-C","./plugin/BASIC_lang/"],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        _ = await process_output(p, m, msg, ctx)
        await ctx.send("ENDED.")
    
    @commands.command(aliases=["basic"])
    async def runbasic(self, ctx):
        if ctx.message.content[:11] != ',basic\n```\n':
            return
        
        if ctx.message.content[-3:] != '```':
            return
        
        program = ctx.message.content[11:-3]
        
        with open('./plugin/BASIC_lang/test.txt', 'w') as f:
            f.write(program)

        p = subprocess.Popen(["make","-C","./plugin/BASIC_lang/"],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        p = subprocess.Popen(["chmod","a+x","./plugin/BASIC_lang/HappyBasic"],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        p = subprocess.Popen(["./plugin/BASIC_lang/HappyBasic -safety ./plugin/BASIC_lang/HappyBasic/script.txt"],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        msg,m = await process_output(p, await ctx.send("RUN DEBUG"), "```basic\n", ctx)
        await m.edit(content=msg+"\n```")
        _ = p.communicate()[0]
        await ctx.send("exit :"+str(p.returncode))


    # @commands.command(aliases=["db"])
    # async def debugbasic(self, ctx):
    #     msg = "DEBUG FROM BASIC LANG IN C++\n"
    #     m: discord.Message = await ctx.send(msg)
    #     p = subprocess.Popen(["make","-C","./plugin/BASIC_lang/"],
    #                          stdout=subprocess.PIPE,
    #                          stderr=subprocess.STDOUT)
    #     _ = await process_output(p, m, msg, ctx)
    #     await ctx.send("ENDED.")
    #     msg = "chmod a+x ./basic\n"
    #     m: discord.Message = await ctx.send(msg)
    #     p = subprocess.Popen(["chmod","a+x","./plugin/BASIC_lang/HappyBasic"],
    #                          stdout=subprocess.PIPE,
    #                          stderr=subprocess.STDOUT)
    #     _ = await process_output(p, m, msg, ctx)
    #     await ctx.send("ENDED.")
    #     msg = "RUN DEBUG\n"
    #     m: discord.Message = await ctx.send(msg)
    #     p = subprocess.Popen(["./plugin/BASIC_lang/HappyBasic"],
    #                          stdout=subprocess.PIPE,
    #                          stderr=subprocess.STDOUT)
    #     _ = await process_output(p, m, msg, ctx)
    #     _ = p.communicate()[0]
    #     await ctx.send("ENDED. CODE:"+str(p.returncode))


def setup(bot):
    bot.add_cog(msg(bot))
